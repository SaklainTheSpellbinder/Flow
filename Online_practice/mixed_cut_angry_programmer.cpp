#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

// --- YOUR TEMPLATE ---
struct Edge{
    int to, capacity, flow, rev;
};

vector<vector<Edge>> adj;
vector<int> par_node, par_edge_idx;

bool bfs(int s, int t, int n){
    par_node.assign(n, -1);
    par_edge_idx.assign(n, -1);
    queue<int> q;
    q.push(s);
    par_node[s] = s;
    while(!q.empty()){
        int u = q.front(); q.pop();
        for(int i=0; i<adj[u].size(); i++){
            Edge& e = adj[u][i];
            if(par_node[e.to]==-1 && e.capacity-e.flow>0){
                par_node[e.to]=u;
                par_edge_idx[e.to]=i;
                q.push(e.to);
                if(e.to==t) return true;
            }
        }
    }
    return false;
}

ll edmond_karp(int s, int t, int n){
    ll max_flow=0;
    while(bfs(s,t,n)){
        int bottleneck=1e9;
        for(int v=t; v!=s; v=par_node[v]){
            int u=par_node[v];
            int idx=par_edge_idx[v];
            bottleneck=min(bottleneck, adj[u][idx].capacity-adj[u][idx].flow);
        }
        for(int v=t; v!=s; v=par_node[v]){
            int u=par_node[v];
            int idx=par_edge_idx[v];
            int ridx=adj[u][idx].rev;
            adj[u][idx].flow+=bottleneck;
            adj[v][ridx].flow-=bottleneck;
        }
        max_flow+=(ll)bottleneck;
    }
    return max_flow;
}

void add_edge(int u, int v, int cap) {
    int uidx = adj[u].size();
    int vidx = adj[v].size();
    adj[u].pb({v, cap, 0, vidx});
    adj[v].pb({u, 0, 0, uidx});
}

// --- PROBLEM SOLUTION ---
void solve(int case_num) {
    int M_nodes, W_wires;
    cin >> M_nodes >> W_wires;

    // Nodes needed: 
    // 1 to M are IN-nodes
    // M+1 to 2M are OUT-nodes
    // Total Size: 2*M + 5
    int MAX_NODES = 2 * M_nodes + 5;
    adj.assign(MAX_NODES, vector<Edge>());

    // 1. Read Machine Costs (Vertex Capacities)
    // Machines 2 to M-1 have costs. 1 and M are INF.
    vector<int> cost(M_nodes + 1, 0);
    
    // Default 1 and M to Infinite
    cost[1] = 1e9;
    cost[M_nodes] = 1e9;

    // Read intermediate costs
    for(int i=0; i < M_nodes - 2; i++) {
        int id, c; cin >> id >> c;
        cost[id] = c;
    }

    // 2. Build Vertex Split Edges (IN -> OUT)
    for(int i = 1; i <= M_nodes; i++) {
        // u_in = i, u_out = i + M_nodes
        add_edge(i, i + M_nodes, cost[i]);
    }

    // 3. Build Wires (Connecting OUT -> IN)
    for(int i = 0; i < W_wires; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        // Bidirectional Wire:
        // u_out -> v_in
        add_edge(u + M_nodes, v, w);
        // v_out -> u_in
        add_edge(v + M_nodes, u, w);
    }

    // Source: 1 (Boss IN) -> Technically should start at 1_OUT, but since cap is INF, 1_IN is fine.
    // Sink: M + M_nodes (Server OUT) -> Flow must pass through Server's internal edge
    cout << "Case " << case_num << ": " << edmond_karp(1, M_nodes + M_nodes, MAX_NODES) << nl;
}

int main() {
    FAST_IO;
    int t; if(cin >> t) for(int i=1; i<=t; i++) solve(i);
    return 0;
}