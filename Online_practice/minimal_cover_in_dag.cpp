/**
 * Problem: Min Path Cover in DAG (LightOJ 1403 - Air Raid)
 * Logic: Answer = Total Nodes - Max Bipartite Matching
 */
#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

// --- MAX FLOW TEMPLATE ---
struct Edge { int to, capacity, flow, rev; };
vector<vector<Edge>> adj;
vector<int> par_node, par_edge_idx;

bool bfs(int s, int t, int n) {
    par_node.assign(n, -1); par_edge_idx.assign(n, -1);
    queue<int> q; q.push(s); par_node[s] = s;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int i = 0; i < adj[u].size(); i++) {
            Edge &e = adj[u][i];
            if (par_node[e.to] == -1 && e.capacity - e.flow > 0) {
                par_node[e.to] = u; par_edge_idx[e.to] = i; q.push(e.to);
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
            int u = par_node[v]; int idx = par_edge_idx[v];
            bottleneck = min(bottleneck, adj[u][idx].capacity - adj[u][idx].flow);
        }
        for (int v = t; v != s; v = par_node[v]) {
            int u = par_node[v]; int idx = par_edge_idx[v]; int ridx = adj[u][idx].rev;
            adj[u][idx].flow += bottleneck; adj[v][ridx].flow -= bottleneck;
        }
        max_flow += bottleneck;
    }
    return max_flow;
}

void add_edge(int u, int v, int cap) {
    int uidx = adj[u].size(); int vidx = adj[v].size();
    adj[u].pb({v, cap, 0, vidx}); adj[v].pb({u, 0, 0, uidx});
}

// --- SOLUTION ---
void solve(int case_num) {
    int N, M; // N junctions, M roads
    cin >> N >> M;

    // Node Mapping:
    // 1..N: Left Side (Out-nodes)
    // N+1..2N: Right Side (In-nodes)
    // Source: 0
    // Sink: 2N+1
    int S = 0;
    int T = 2 * N + 1;
    int MAX_NODES = T + 5;
    adj.assign(MAX_NODES, vector<Edge>());

    // 1. Source -> Left Side
    for (int i = 1; i <= N; i++) {
        add_edge(S, i, 1);
    }

    // 2. Right Side -> Sink
    for (int i = 1; i <= N; i++) {
        add_edge(N + i, T, 1);
    }

    // 3. DAG Edges (u -> v becomes u_Left -> v_Right)
    for (int i = 0; i < M; i++) {
        int u, v; cin >> u >> v;
        add_edge(u, N + v, 1);
    }

    // 4. Calculate Max Matching
    int matching = edmond_karp(S, T, MAX_NODES);

    // 5. Result = Total Nodes - Matching
    cout << "Case " << case_num << ": " << (N - matching) << nl;
}

int main() {
    FAST_IO;
    int t; if (cin >> t) for (int i = 1; i <= t; i++) solve(i);
    return 0;
}