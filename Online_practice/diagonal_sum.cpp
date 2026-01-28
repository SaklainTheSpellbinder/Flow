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

// Helper to track which edge belongs to which cell (r, c)
struct CellEdge {
    int u_node;
    int edge_index;
};
vector<vector<CellEdge>> grid_map;

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

// Helper to add edges easily using your style
void add_edge(int u, int v, int cap, int r = -1, int c = -1) {
    int uidx = adj[u].size();
    int vidx = adj[v].size();
    
    // If this edge represents a grid cell, save its location so we can check flow later
    if(r != -1) {
        grid_map[r][c] = {u, uidx};
    }
    
    adj[u].pb({v, cap, 0, vidx});
    adj[v].pb({u, 0, 0, uidx});
}

void solve(int case_num) {
    int N, M;
    if(!(cin >> N >> M)) return;

    int num_diag = N + M - 1;
    
    // Indices for sums
    vector<int> sum1(num_diag + 1); // i+j sums
    vector<int> sum2(num_diag + 1); // i-j sums

    for(int i = 1; i <= num_diag; i++) cin >> sum1[i];
    for(int i = 1; i <= num_diag; i++) cin >> sum2[i];

    // Node Calculation
    // Source = 0
    // Sum1 Nodes (Left): 1 to num_diag
    // Sum2 Nodes (Right): num_diag + 1 to 2*num_diag
    // Sink = 2*num_diag + 1
    
    int S = 0;
    int T = 2 * num_diag + 1;
    int MAX_NODES = T + 5;

    adj.assign(MAX_NODES, vector<Edge>());
    grid_map.assign(N + 1, vector<CellEdge>(M + 1));

    // We need to count how many cells are in each diagonal to adjust the lower bound
    // Lower Bound = 1. So we subtract 1 from sum for every cell.
    vector<int> count1(num_diag + 1, 0);
    vector<int> count2(num_diag + 1, 0);

    // Lambda to map (r, c) to diagonal IDs
    auto get_d1 = [&](int r, int c) { return r + c - 1; }; 
    auto get_d2 = [&](int r, int c) { return r - c + M; }; 

    // 1. Pre-calculate counts
    for(int r = 1; r <= N; r++) {
        for(int c = 1; c <= M; c++) {
            count1[get_d1(r, c)]++;
            count2[get_d2(r, c)]++;
        }
    }

    // 2. Build Source -> Left Nodes
    for(int i = 1; i <= num_diag; i++) {
        // Adjusted Capacity = TargetSum - (1 * cell_count)
        int cap = sum1[i] - count1[i];
        add_edge(S, i, cap);
    }

    // 3. Build Right Nodes -> Sink
    for(int i = 1; i <= num_diag; i++) {
        int cap = sum2[i] - count2[i];
        add_edge(num_diag + i, T, cap);
    }

    // 4. Build Middle Edges (The Grid Cells)
    for(int r = 1; r <= N; r++) {
        for(int c = 1; c <= M; c++) {
            int u = get_d1(r, c);
            int v = num_diag + get_d2(r, c);
            // Capacity 8 because range is 1..9 (so 0..8 flow)
            add_edge(u, v, 8, r, c); 
        }
    }

    // 5. Run Flow
    edmond_karp(S, T, MAX_NODES);

    // 6. Output
    cout << "Case " << case_num << ":" << endl;
    for(int r = 1; r <= N; r++) {
        for(int c = 1; c <= M; c++) {
            CellEdge ref = grid_map[r][c];
            int val = adj[ref.u_node][ref.edge_index].flow;
            // Add 1 back to get range 1..9
            cout << (val + 1) << (c == M ? "" : " ");
        }
        cout << endl;
    }
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