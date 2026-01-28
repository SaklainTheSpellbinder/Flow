/**
 * Problem: LightOJ 1150 - Ghosts!
 * Approach: Grid BFS + Binary Search + Bipartite Matching
 */
#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

struct Edge{ int to, capacity, flow, rev; };
vector<vector<Edge>> adj;
vector<int> par_node, par_edge_idx;

bool bfs_flow(int s, int t, int n){
    par_node.assign(n, -1); par_edge_idx.assign(n, -1);
    queue<int> q; q.push(s); par_node[s]=s;
    while(!q.empty()){
        int u=q.front(); q.pop();
        for(int i=0; i<adj[u].size(); i++){
            Edge& e=adj[u][i];
            if(par_node[e.to]==-1 && e.capacity-e.flow>0){
                par_node[e.to]=u; par_edge_idx[e.to]=i; q.push(e.to);
                if(e.to==t) return true;
            }
        }
    }
    return false;
}

ll edmond_karp(int s, int t, int n){
    ll max_flow=0;
    while(bfs_flow(s,t,n)){
        int bottleneck=1e9;
        for(int v=t; v!=s; v=par_node[v]){
            int u=par_node[v]; int idx=par_edge_idx[v];
            bottleneck=min(bottleneck, adj[u][idx].capacity-adj[u][idx].flow);
        }
        for(int v=t; v!=s; v=par_node[v]){
            int u=par_node[v]; int idx=par_edge_idx[v]; int ridx=adj[u][idx].rev;
            adj[u][idx].flow+=bottleneck; adj[v][ridx].flow-=bottleneck;
        }
        max_flow+=(ll)bottleneck;
    }
    return max_flow;
}

void add_edge(int u, int v, int cap) {
    int uidx=adj[u].size(); int vidx=adj[v].size();
    adj[u].pb({v, cap, 0, vidx}); adj[v].pb({u, 0, 0, uidx});
}

// --- PROBLEM SPECIFIC ---
struct Point { int r, c; };
vector<Point> ghosts, humans;
int dist_mat[55][55]; // [ghost_id][human_id]
int dr[] = {0, 0, 1, -1};
int dc[] = {1, -1, 0, 0};

// Grid BFS: Measures distance from ONE ghost to ALL humans
void bfs_grid(int g_idx, int N, vector<string>& grid) {
    int d[N][N]; memset(d, -1, sizeof(d));
    queue<Point> q;
    q.push(ghosts[g_idx]);
    d[ghosts[g_idx].r][ghosts[g_idx].c] = 0;

    while(!q.empty()) {
        Point u = q.front(); q.pop();
        for(int i=0; i<4; i++) {
            int nr = u.r + dr[i], nc = u.c + dc[i];
            if(nr>=0 && nr<N && nc>=0 && nc<N && grid[nr][nc]!='#' && d[nr][nc]==-1) {
                d[nr][nc] = d[u.r][u.c] + 1;
                q.push({nr, nc});
            }
        }
    }
    for(int h=0; h<humans.size(); h++) {
        int steps = d[humans[h].r][humans[h].c];
        // Time = steps * 2 + 2 (Based on typical interpretation of this problem)
        // If simply distance, remove the *2+2 logic.
        if(steps != -1) dist_mat[g_idx][h] = steps * 2 + 2; 
        else dist_mat[g_idx][h] = 1e9;
    }
}

bool check(int time, int G, int H) {
    // Source: 0
    // Ghosts: 1 to G
    // Humans: G+1 to G+H
    // Sink: G+H+1
    int S = 0, T = G + H + 1;
    adj.assign(T + 5, vector<Edge>());

    // Source -> Ghosts (Cap 1)
    for(int i=0; i<G; i++) add_edge(S, i+1, 1);
    
    // Humans -> Sink (Cap 1)
    for(int i=0; i<H; i++) add_edge(G+i+1, T, 1);

    // Ghost -> Human (if reachable within time)
    for(int i=0; i<G; i++) {
        for(int j=0; j<H; j++) {
            if(dist_mat[i][j] <= time) {
                add_edge(i+1, G+j+1, 1);
            }
        }
    }
    // All ghosts must be matched
    return edmond_karp(S, T, T+5) == G;
}

void solve(int case_num) {
    int N; cin >> N;
    vector<string> grid(N);
    ghosts.clear(); humans.clear();
    for(int i=0; i<N; i++) {
        cin >> grid[i];
        for(int j=0; j<N; j++) {
            if(grid[i][j] == 'G') ghosts.pb({i, j});
            if(grid[i][j] == 'H') humans.pb({i, j});
        }
    }

    // Precalculate distances
    for(int i=0; i<ghosts.size(); i++) bfs_grid(i, N, grid);

    // Binary Search on Time
    int low = 0, high = 2 * N * N * 2 + 100, ans = -1;
    while(low <= high) {
        int mid = low + (high - low) / 2;
        if(check(mid, ghosts.size(), humans.size())) {
            ans = mid; high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    cout << "Case " << case_num << ": ";
    if(ans == -1) cout << "Vuter Dol Kupokat" << nl;
    else cout << ans << nl;
}

int main() {
    FAST_IO;
    int t; if(cin >> t) for(int i=1; i<=t; i++) solve(i);
    return 0;
}