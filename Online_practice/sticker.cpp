/**
 * Problem: Sticker Exchange (Max Flow)
 * Source: Online on Max flow - CSE 208 (Jan 2023) - A2
 */
#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

// --- MAX FLOW TEMPLATE ---
struct Edge {
    int to, capacity, flow, rev;
    int original_sticker_id; // Helper to track which sticker this edge carries
};

vector<vector<Edge>> adj;
vector<int> par_node, par_edge_idx;

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
        max_flow += (ll)bottleneck;
    }
    return max_flow;
}

// Helper to add edges (with optional sticker tracking)
void add_edge(int u, int v, int cap, int sticker_id = -1) {
    int uidx = adj[u].size();
    int vidx = adj[v].size();
    adj[u].pb({v, cap, 0, vidx, sticker_id});
    adj[v].pb({u, 0, 0, uidx, -1});
}

// --- PROBLEM SOLUTION ---
void solve() {
    int N, M; // N friends, M sticker types
    if (!(cin >> N >> M)) return;

    // Node Mapping
    // Source = 0
    // Stickers = 1 to M
    // Friends = M + 1 to M + N
    // Sink = M + N + 1
    int S = 0;
    int T = M + N + 1;
    int MAX_NODES = T + 5;
    adj.assign(MAX_NODES, vector<Edge>());

    // 1. My Stickers
    int my_count; cin >> my_count;
    vector<int> my_stickers(M + 1, 0);
    for (int i = 0; i < my_count; i++) {
        int s_id; cin >> s_id;
        my_stickers[s_id]++;
    }

    // Edge: Source -> Sticker_i (Cap = My Count) 
    for (int i = 1; i <= M; i++) {
        if (my_stickers[i] > 0) {
            add_edge(S, i, my_stickers[i]);
        }
    }

    // 2. Friends' Stickers
    vector<vector<int>> friend_stickers(N + 1, vector<int>(M + 1, 0));
    for (int j = 1; j <= N; j++) {
        int f_count; cin >> f_count;
        for (int k = 0; k < f_count; k++) {
            int s_id; cin >> s_id;
            friend_stickers[j][s_id]++;
        }
    }

    // 3. Build Trading Edges
    for (int s_id = 1; s_id <= M; s_id++) {
        // Edge: Sticker_i -> Sink (Cap 1) 
        // This ensures we count DISTINCT stickers collected
        add_edge(s_id, T, 1);

        for (int f_id = 1; f_id <= N; f_id++) {
            int friend_node = M + f_id;

            // Edge: Sticker_i -> Friend_j (Cap 1)
            // Condition: Friend doesn't have it [cite: 200]
            if (friend_stickers[f_id][s_id] == 0) {
                add_edge(s_id, friend_node, 1, s_id); // Track s_id for printing later
            }

            // Edge: Friend_j -> Sticker_i (Cap = Count - 1)
            // Condition: Friend has duplicates 
            if (friend_stickers[f_id][s_id] > 1) {
                int spare_copies = friend_stickers[f_id][s_id] - 1;
                add_edge(friend_node, s_id, spare_copies, s_id);
            }
        }
    }

    // 4. Run Max Flow
    cout << edmond_karp(S, T, MAX_NODES) << nl;

    // 5. Print Exchanges 
    // Iterate over all friends to see what passed through them
    for (int f_id = 1; f_id <= N; f_id++) {
        int friend_node = M + f_id;
        
        vector<int> received; // What I gave to friend
        vector<int> given_back; // What friend gave me

        // Check incoming edges to Friend (Sticker -> Friend)
        // In adjacency list, these are stored at Sticker nodes. 
        // We can check the REVERSE edges at the Friend node to find them easily,
        // or iterate all stickers. Iterating stickers is safer.
        for(int s=1; s<=M; s++) {
             for(auto& e : adj[s]) {
                 if(e.to == friend_node && e.flow == 1) {
                     received.pb(s);
                 }
             }
        }

        // Check outgoing edges from Friend (Friend -> Sticker)
        for(auto& e : adj[friend_node]) {
            if(e.to >= 1 && e.to <= M && e.flow > 0) {
                // Because capacity > 1 is possible, push multiple times
                for(int k=0; k < e.flow; k++) {
                    given_back.pb(e.original_sticker_id);
                }
            }
        }

        // Match them up
        int trades = min(received.size(), given_back.size());
        for(int i=0; i<trades; i++) {
            cout << "Exchange a sticker with number " << received[i] 
                 << " against a sticker with number " << given_back[i] 
                 << " of friend " << f_id << nl;
        }
    }
}

int main() {
    FAST_IO;
    solve();
    return 0;
}