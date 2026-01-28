/**
 * Problem: Owls and Mice (Geometric Matching)
 * Source: Maxflow_OnlineB.pdf
 */
#include <bits/stdc++.h>
using namespace std;

#define pb push_back
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

// --- PROBLEM SPECIFIC ---
struct Point { double x, y; };
struct Hole { double x, y; int cap; };

// Euclidean Distance
double get_dist(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

void solve(int case_num) {
    int M, H; // M mice, H holes
    double R; // Max Run Distance
    cin >> M >> H >> R;

    vector<Point> mice(M);
    for (int i = 0; i < M; i++) cin >> mice[i].x >> mice[i].y;

    vector<Hole> holes(H);
    for (int i = 0; i < H; i++) cin >> holes[i].x >> holes[i].y >> holes[i].cap;

    // Node Mapping:
    // Source = 0
    // Mice = 1 to M
    // Holes = M+1 to M+H
    // Sink = M+H+1
    int S = 0;
    int T = M + H + 1;
    int MAX_NODES = T + 5;
    adj.assign(MAX_NODES, vector<Edge>());

    // 1. Source -> Mice (Cap 1)
    for (int i = 0; i < M; i++) add_edge(S, i + 1, 1);

    // 2. Holes -> Sink (Cap = Hole Capacity)
    for (int j = 0; j < H; j++) add_edge(M + j + 1, T, holes[j].cap);

    // 3. Mice -> Holes (If reachable)
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < H; j++) {
            if (get_dist(mice[i].x, mice[i].y, holes[j].x, holes[j].y) <= R + 1e-9) {
                // Mouse (i+1) -> Hole (M+j+1)
                add_edge(i + 1, M + j + 1, 1);
            }
        }
    }

    cout << "Case " << case_num << ": " << edmond_karp(S, T, MAX_NODES) << nl;
}

int main() {
    FAST_IO;
    int t; if (cin >> t) for (int i = 1; i <= t; i++) solve(i);
    return 0;
}