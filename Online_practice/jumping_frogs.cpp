#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define ld long double
#define mp make_pair
#define nl "\n"

#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);

struct Edge{
    int to;
    int capacity;
    int flow;
    int rev;
};

vector<vector<Edge>> adj;
vector<int> par_node;
vector<int> par_edge_idx;

// Standard BFS for Edmonds-Karp
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

// Standard Edmonds-Karp
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

// Helper to add edges easily
void add_edge(int u, int v, int cap) {
    int uidx = adj[u].size();
    int vidx = adj[v].size();
    adj[u].pb({v, cap, 0, vidx});
    adj[v].pb({u, 0, 0, uidx});
}

struct Rock {
    int dist;
    char type; // 'B' or 'S'
};

// --- THE CHECK FUNCTION ---
// Returns TRUE if we can send 2 units of flow with max jump 'limit'
bool check(int limit, int N, int Width, vector<Rock>& rocks) {
    // 1. Reset Graph
    // Nodes: Source(0), Sink(2*N+1)
    // Rocks: i splits into (2*i+1) and (2*i+2)
    int MAX_NODES = 2 * N + 5;
    adj.assign(MAX_NODES, vector<Edge>());
    
    int S = 0;
    int T = 2 * N + 1;

    // 2. Build Graph based on 'limit'
    
    // A. Source -> Rocks
    for(int i = 0; i < N; i++) {
        if(rocks[i].dist <= limit) {
            // Source connects to IN-node of rock i
            add_edge(S, 2*i + 1, 1e9);
        }
    }
    // A2. Source -> Sink (Direct Jump)
    if(Width <= limit) add_edge(S, T, 1e9);

    // B. Rock -> Rock & Internal Capacities
    for(int i = 0; i < N; i++) {
        // Internal Edge: IN(2*i+1) -> OUT(2*i+2)
        int cap = (rocks[i].type == 'B') ? 1e9 : 1;
        add_edge(2*i + 1, 2*i + 2, cap);

        // Jump to Sink?
        if(Width - rocks[i].dist <= limit) {
            add_edge(2*i + 2, T, 1e9);
        }

        // Jump to other Rocks?
        for(int j = i + 1; j < N; j++) {
            if(rocks[j].dist - rocks[i].dist <= limit) {
                // From i_OUT to j_IN
                add_edge(2*i + 2, 2*j + 1, 1e9);
            }
        }
    }

    // 3. Check if Max Flow >= 2
    return edmond_karp(S, T, MAX_NODES) >= 2;
}

void solve(int case_num) {
    int N, Width;
    cin >> N >> Width;

    vector<Rock> rocks(N);
    for(int i = 0; i < N; i++) {
        string s; cin >> s;
        // Parse "B-10" or "S-20"
        rocks[i].type = s[0];
        // Extract number after '-'
        rocks[i].dist = stoi(s.substr(2));
    }

    // Binary Search on Answer
    int low = 0, high = Width, ans = Width;
    
    while(low <= high) {
        int mid = low + (high - low) / 2;
        if(check(mid, N, Width, rocks)) {
            ans = mid;
            high = mid - 1; // Try smaller distance
        } else {
            low = mid + 1;  // Need bigger jumps
        }
    }

    cout << "Case " << case_num << ": " << ans << nl;
}

int main() {
    FAST_IO;
    int t;
    if(cin >> t) {
        for(int i = 1; i <= t; i++) {
            solve(i);
        }
    }
    return 0;
}