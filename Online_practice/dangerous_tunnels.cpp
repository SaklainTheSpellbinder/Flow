#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

struct Edge{ int to, capacity, flow, rev; };
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
    adj[v].pb({u, 0, 0, uidx}); // Directed graph logic (reverse cap 0)
}

struct Tunnel { int u, v, w; };

bool check(int limit, int N, int K, vector<Tunnel>& tunnels) {
    // Reset Graph. Nodes 0 to N+1.
    int MAX_NODES = N + 5;
    adj.assign(MAX_NODES, vector<Edge>());

    for(auto& t : tunnels) {
        if(t.w <= limit) {
            // Directed edge with Capacity 1
            add_edge(t.u, t.v, 1);
        }
    }
    // Check if K units can flow from 0 to N+1
    return edmond_karp(0, N + 1, MAX_NODES) >= K;
}

void solve(int case_num) {
    int N, E, K; // Rooms 1..N. Start 0, End N+1.
    cin >> N >> E >> K;

    vector<Tunnel> tunnels;
    int max_danger = 0;
    for(int i=0; i<E; i++) {
        int u, v, w; cin >> u >> v >> w;
        tunnels.pb({u, v, w});
        max_danger = max(max_danger, w);
    }

    int low = 0, high = max_danger, ans = -1;
    while(low <= high) {
        int mid = low + (high - low) / 2;
        if(check(mid, N, K, tunnels)) {
            ans = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    cout << "Case " << case_num << ": ";
    if(ans == -1) cout << "no solution" << nl;
    else cout << ans << nl;
}

int main() {
    FAST_IO;
    int t; if(cin >> t) for(int i=1; i<=t; i++) solve(i);
    return 0;
}