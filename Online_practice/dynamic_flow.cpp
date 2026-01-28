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

// --- DYNAMIC FLOW SOLVER ---
struct Road {
    int u, v, cap, time;
};

void solve(int case_num) {
    int N, M, K, S, T;
    // Input: Nodes, Roads, Time Limit, Source, Sink
    cin >> N >> M >> K >> S >> T;
    
    // Convert 1-based input to 0-based for easier logic
    S--; T--; 

    vector<Road> roads;
    for(int i=0; i<M; i++) {
        int u, v, c, d;
        cin >> u >> v >> c >> d;
        roads.pb({u-1, v-1, c, d}); // 0-based conversion
    }

    // --- NODE MAPPING ---
    // Node 'u' at time 't' will have ID: u + (t * N)
    // Layers go from t=0 to t=K.
    // Total Layers = K + 1.
    // Total physical nodes needed = N * (K + 1).
    // Plus one "Super Sink" node at the very end.
    
    int nodes_per_layer = N;
    int total_layers = K + 1;
    int total_graph_nodes = nodes_per_layer * total_layers;
    int SuperSink = total_graph_nodes; 

    // Resize Adjacency List (Safe size)
    adj.assign(SuperSink + 5, vector<Edge>());

    // 1. Build "Holdover" Edges (Waiting in the same room)
    // You can move from (Room u at Time t) -> (Room u at Time t+1)
    for(int t = 0; t < K; t++) {
        for(int u = 0; u < N; u++) {
            int u_now = u + (t * N);
            int u_next = u + ((t + 1) * N);
            
            // Capacity is Infinite (people can wait freely)
            // Or limit it if rooms have max capacity.
            add_edge(u_now, u_next, 1e9);
        }
    }

    // 2. Build "Travel" Edges (Moving between rooms)
    // Road u->v takes 'time' minutes.
    // You can leave u at Time t, and arrive at v at Time (t + time).
    for(auto& r : roads) {
        for(int t = 0; t <= K - r.time; t++) {
            int u_start = r.u + (t * N);
            int v_arrive = r.v + ((t + r.time) * N);
            
            add_edge(u_start, v_arrive, r.cap);
        }
    }

    // 3. Connect Real Sink to Super Sink
    // Being at the Sink (Safe Zone) at ANY time t means you are safe.
    for(int t = 0; t <= K; t++) {
        int sink_at_t = T + (t * N);
        add_edge(sink_at_t, SuperSink, 1e9);
    }

    // 4. Run Max Flow
    // Start from Source at Time 0
    int real_source = S + (0 * N);
    
    // Calculate Flow to Super Sink
    ll result = edmond_karp(real_source, SuperSink, SuperSink + 5);

    cout << "Case " << case_num << ": " << result << nl;
}

int main() {
    FAST_IO;
    int t; 
    if(cin >> t) {
        for(int i = 1; i <= t; i++) solve(i);
    }
    return 0;
}