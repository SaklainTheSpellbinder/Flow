#include <bits/stdc++.h>
using namespace std;

#define ll long long
const ll INF = 1e18; // Use a large number for infinity

struct Edge {
    int to;
    int capacity;
    int flow;
    int cost; // <--- NEW: Cost per unit
    int rev;
};

vector<vector<Edge>> adj;
vector<int> par_node;
vector<int> par_edge_idx;
vector<ll> dist; // Stores the cheapest cost to reach each node

// Standard Bellman-Ford Algorithm
// Returns true if there is a path from s to t that can accept flow
bool bellman_ford(int s, int t, int n) {
    par_node.assign(n, -1);
    par_edge_idx.assign(n, -1);
    dist.assign(n, INF);

    dist[s] = 0;
    vector<bool> in_queue(n, false); // Optimization not needed for pure BF, but helps reading

    // Relax edges N-1 times
    // (We iterate loop N times purely to check for reachability properly)
    for (int k = 0; k < n; k++) {
        bool changed = false;
        for (int u = 0; u < n; u++) {
            if (dist[u] == INF) continue; // Cannot relax from an unreachable node

            for (int i = 0; i < adj[u].size(); i++) {
                Edge &e = adj[u][i];
                
                // Relaxation Condition:
                // 1. Edge has remaining capacity
                // 2. Finding a cheaper path
                if (e.capacity - e.flow > 0 && dist[e.to] > dist[u] + e.cost) {
                    dist[e.to] = dist[u] + e.cost;
                    par_node[e.to] = u;
                    par_edge_idx[e.to] = i;
                    changed = true;
                }
            }
        }
        // Optimization: If no distances changed in a full round, we are done early
        if (!changed) break; 
    }

    // Check if we actually reached the sink
    return dist[t] != INF;
}

pair<ll, ll> min_cost_max_flow(int s, int t, int n) {
    ll max_flow = 0;
    ll min_cost = 0;

    // While a valid path exists (found by Bellman-Ford)
    while (bellman_ford(s, t, n)) {
        int bottleneck = 1e9;

        // 1. Find the bottleneck (min capacity) along the path
        for (int v = t; v != s; v = par_node[v]) {
            int u = par_node[v];
            int idx = par_edge_idx[v];
            bottleneck = min(bottleneck, adj[u][idx].capacity - adj[u][idx].flow);
        }

        // 2. Push flow and update total cost
        for (int v = t; v != s; v = par_node[v]) {
            int u = par_node[v];
            int idx = par_edge_idx[v];
            int ridx = adj[u][idx].rev;

            // Forward Edge
            adj[u][idx].flow += bottleneck;
            
            // Reverse Edge
            adj[v][ridx].flow -= bottleneck;
        }

        max_flow += bottleneck;
        // Cost = Amount of Flow * Cost per Unit along this path (dist[t])
        min_cost += (ll)bottleneck * dist[t]; 
    }
    return {max_flow, min_cost};
}

int main() {
    // Input Handling
    int N, M;
    if (!(cin >> N >> M)) return 0;
    
    adj.assign(N, vector<Edge>());

    for (int i = 0; i < M; i++) {
        int u, v, cap, cost;
        cin >> u >> v >> cap >> cost;
        
        int uidx = adj[u].size();
        int vidx = adj[v].size();

        // Forward Edge: Capacity=cap, Cost=cost
        adj[u].push_back({v, cap, 0, cost, vidx});
        
        // Reverse Edge: Capacity=0, Cost = -cost (NEGATIVE!)
        adj[v].push_back({u, 0, 0, -cost, uidx}); 
    }

    int s, t;
    cin >> s >> t;

    pair<ll, ll> res = min_cost_max_flow(s, t, N);
    
    cout << "Max Flow: " << res.first << endl;
    cout << "Min Cost: " << res.second << endl;

    return 0;
}