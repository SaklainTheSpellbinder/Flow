/*
 * Problem: Operation Royal Mint (Edge Disjoint Paths + Printing)
 * Logic: Max Flow with Capacity 1 + DFS Path Recovery
 */
#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

struct Edge {
    int to, capacity, flow, rev;
};

vector<vector<Edge>> adj;
vector<int> par_node, par_edge_idx;

// --- Standard BFS & Edmonds-Karp (Your Template) ---
bool bfs(int s, int t, int n) {
    par_node.assign(n + 1, -1);
    par_edge_idx.assign(n + 1, -1);
    queue<int> q;
    q.push(s);
    par_node[s] = s;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int i = 0; i < adj[u].size(); i++) {
            Edge &e = adj[u][i];
            if (par_node[e.to] == -1 && e.capacity - e.flow > 0) {
                par_node[e.to] = u;
                par_edge_idx[e.to] = i;
                q.push(e.to);
                if (e.to == t) return true;
            }
        }
    }
    return false;
}

int edmond_karp(int s, int t, int n) {
    int max_flow = 0;
    while (bfs(s, t, n)) {
        int bottleneck = 1e9;
        for (int v = t; v != s; v = par_node[v]) {
            int u = par_node[v];
            int idx = par_edge_idx[v];
            bottleneck = min(bottleneck, adj[u][idx].capacity - adj[u][idx].flow);
        }
        for (int v = t; v != s; v = par_node[v]) {
            int u = par_node[v];
            int idx = par_edge_idx[v];
            int ridx = adj[u][idx].rev;
            adj[u][idx].flow += bottleneck;
            adj[v][ridx].flow -= bottleneck;
        }
        max_flow += bottleneck;
    }
    return max_flow;
}

void add_edge(int u, int v) {
    int uidx = adj[u].size();
    int vidx = adj[v].size();
    // Capacity is always 1 for Disjoint Paths
    adj[u].pb({v, 1, 0, vidx});
    adj[v].pb({u, 0, 0, uidx});
}

// --- NEW FUNCTION: Print Paths ---
// DFS to find a path using edges with flow=1
void print_one_path(int u, int target, vector<int>& path) {
    path.pb(u);
    if (u == target) return;

    for (auto& e : adj[u]) {
        // If this edge carried flow, it's part of a path
        if (e.flow == 1) {
            e.flow = 0; // Mark as visited so we don't reuse it for the next path
            print_one_path(e.to, target, path);
            return; 
        }
    }
}

void solve(int case_num) {
    int N, M;
    cin >> N >> M;

    // Reset Graph
    adj.assign(N + 1, vector<Edge>());

    for (int i = 0; i < M; i++) {
        int u, v; cin >> u >> v;
        add_edge(u, v);
    }

    int max_routes = edmond_karp(1, N, N);

    cout << "Case " << case_num << ": " << max_routes << nl;

    if (max_routes == 0) {
        cout << "No escape route possible! The Professor needs a new plan." << nl;
    } else {
        // Print exactly 'max_routes' paths
        for (int i = 0; i < max_routes; i++) {
            vector<int> path;
            print_one_path(1, N, path);
            
            // Output format: 1->2->4
            for (int j = 0; j < path.size(); j++) {
                cout << path[j] << (j == path.size() - 1 ? "" : "->");
            }
            cout << nl;
        }
    }
}

int main() {
    FAST_IO;
    int t; if(cin >> t) for(int i=1; i<=t; i++) solve(i);
    return 0;
}