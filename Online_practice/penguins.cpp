#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define ld long double
#define mp make_pair
#define nl "\n"

#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);

// --- YOUR ORIGINAL STRUCTURES & ALGORITHMS (UNCHANGED) ---
struct Edge{
    int to;
    int capacity;
    int flow;
    int rev;
};

vector<vector<Edge>> adj;
vector<int> par_node;
vector<int> par_edge_idx;

// Modified BFS to accept 'n' (node count) for resizing
bool bfs(int s, int t, int n){
    par_node.assign(n, -1);
    par_edge_idx.assign(n, -1);
    queue<int> q;

    q.push(s);
    par_node[s] = s;

    while(!q.empty()){
        int u = q.front();
        q.pop();
        for(int i = 0; i < adj[u].size(); i++){
            Edge& e = adj[u][i];
            if(par_node[e.to] == -1 && e.capacity - e.flow > 0){
                par_node[e.to] = u;
                par_edge_idx[e.to] = i;
                q.push(e.to);
                if(e.to == t) return true;
            }
        }
    }
    return false;
}

ll edmond_karp(int s, int t, int n){
    ll max_flow = 0;
    while(bfs(s, t, n)){
        int bottleneck = 1e9;
        for(int v = t; v != s; v = par_node[v]){
            int u = par_node[v];
            int idx = par_edge_idx[v];
            bottleneck = min(bottleneck, adj[u][idx].capacity - adj[u][idx].flow);
        }

        for(int v = t; v != s; v = par_node[v]){
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

// --- NEW HELPER FUNCTIONS FOR THIS PROBLEM ---

struct Node {
    double x, y;
    int current_penguins, jump_limit;
};

// Helper to add edges easily
void add_edge(int u, int v, int cap) {
    Edge a = {v, cap, 0, (int)adj[v].size()};
    Edge b = {u, 0, 0, (int)adj[u].size()}; // Back edge capacity 0
    adj[u].pb(a);
    adj[v].pb(b);
}

// Squared distance to avoid precision errors with sqrt
double distSq(Node a, Node b) {
    return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y);
}

// --- SOLVE FUNCTION ---
void solve(int case_num) {
    int N;
    double D;
    cin >> N >> D;

    vector<Node> floes(N);
    int total_penguins = 0;

    for(int i=0; i<N; i++) {
        cin >> floes[i].x >> floes[i].y >> floes[i].current_penguins >> floes[i].jump_limit;
        total_penguins += floes[i].current_penguins;
    }

    vector<int> results;

    // KEY CHANGE: Iterative Sink Loop
    // We try every ice floe 'target' as the meeting point
    for(int target = 0; target < N; target++) {
        
        // 1. Reset Graph for this iteration
        // Nodes: Each floe i splits into 2*i (IN) and 2*i+1 (OUT)
        // SuperSource = 2*N
        int MAX_NODES = 2 * N + 5;
        adj.assign(MAX_NODES, vector<Edge>()); // Clear adjacency list
        
        int S = 2 * N;       // Super Source
        int T = 2 * target;  // IMPORTANT: Sink is target_IN (penguins stay here)

        for(int i=0; i<N; i++) {
            int u_in = 2*i;
            int u_out = 2*i + 1;

            // A. SuperSource -> Initial Location (Capacity: count of penguins)
            if(floes[i].current_penguins > 0) {
                add_edge(S, u_in, floes[i].current_penguins);
            }

            // B. Vertex Splitting (Capacity: Jump Limit)
            add_edge(u_in, u_out, floes[i].jump_limit);

            // C. Movement Edges (Between Floes)
            for(int j=0; j<N; j++) {
                if(i == j) continue;
                // If distance allows jump
                if(distSq(floes[i], floes[j]) <= D * D + 1e-9) {
                    int v_in = 2*j;
                    // Jump from u_out -> v_in
                    add_edge(u_out, v_in, 1e9); // Infinite capacity on the air-path
                }
            }
        }

        // 2. Run Max Flow
        if(edmond_karp(S, T, MAX_NODES) == total_penguins) {
            results.pb(target);
        }
    }

    // Output
    cout << "Case " << case_num << ":";
    if(results.empty()) cout << " -1" << nl;
    else {
        for(int id : results) cout << " " << id;
        cout << nl;
    }
}

int main() {
    FAST_IO;
    int t;
    cin >> t;
    for(int i=1; i<=t; i++) {
        solve(i);
    }
    return 0;
}