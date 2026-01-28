#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

struct Edge {
    int to, capacity, flow, rev;
};

vector<vector<Edge>> adj;
vector<int> par_node, par_edge_idx;

// --- BFS & Edmonds-Karp (Standard) ---
bool bfs(int s, int t, int n) {
    par_node.assign(n, -1);
    par_edge_idx.assign(n, -1);
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

ll edmond_karp(int s, int t, int n) {
    ll max_flow = 0;
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

void add_edge(int u, int v, int cap) {
    int uidx = adj[u].size();
    int vidx = adj[v].size();
    adj[u].pb({v, cap, 0, vidx});
    adj[v].pb({u, 0, 0, uidx});
}

struct Rock {
    int dist;
    char type;
};

// --- CHECK FUNCTION (Using +N Indexing) ---
bool check(int limit, int N, int Width, vector<Rock>& rocks) {
    // Total Nodes needed: 
    // 0 (Source) + N (In-Nodes) + N (Out-Nodes) + 1 (Sink) = 2*N + 2
    int MAX_NODES = 2 * N + 2;
    adj.assign(MAX_NODES, vector<Edge>());

    int S = 0;
    int T = 2 * N + 1; // Sink is the last node

    // 1. Build Edges
    // NOTE: Rocks are 0-indexed in vector, but we map them to 1-based Nodes to avoid conflict with S=0
    // Rock i (vector) -> Node (i + 1)
    
    // A. Source -> Rocks (Source -> IN)
    for (int i = 0; i < N; i++) {
        if (rocks[i].dist <= limit) {
            // Connect Source to Rock i's IN-node
            int u_in = i + 1; 
            add_edge(S, u_in, 1e9);
        }
    }
    // A2. Source -> Sink (Direct Jump)
    if (Width <= limit) add_edge(S, T, 1e9);

    // B. Rock Processing
    for (int i = 0; i < N; i++) {
        int u_in = i + 1;
        int u_out = u_in + N; // The +N trick!

        // 1. Internal Edge (Split: IN -> OUT)
        int cap = (rocks[i].type == 'B') ? 1e9 : 1;
        add_edge(u_in, u_out, cap);

        // 2. Jump to Sink (OUT -> Sink)
        if (Width - rocks[i].dist <= limit) {
            add_edge(u_out, T, 1e9);
        }

        // 3. Jump to other Rocks (OUT -> Neighbor IN)
        for (int j = i + 1; j < N; j++) {
            if (rocks[j].dist - rocks[i].dist <= limit) {
                int v_in = j + 1;
                add_edge(u_out, v_in, 1e9);
            }
        }
    }

    // Check if we can push 2 units of flow (Round trip)
    return edmond_karp(S, T, MAX_NODES) >= 2;
}

void solve(int case_num) {
    int N, Width;
    cin >> N >> Width;

    vector<Rock> rocks(N);
    for (int i = 0; i < N; i++) {
        string s; cin >> s;
        // Parse "B-10" -> type='B', dist=10
        rocks[i].type = s[0];
        rocks[i].dist = stoi(s.substr(2));
    }

    int low = 0, high = Width, ans = Width;
    
    // Binary Search on Answer
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (check(mid, N, Width, rocks)) {
            ans = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    cout << "Case " << case_num << ": " << ans << nl;
}

int main() {
    FAST_IO;
    int t;
    if (cin >> t) {
        for (int i = 1; i <= t; i++) solve(i);
    }
    return 0;
}