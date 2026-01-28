// Returns a boolean vector where visited[u] is true if u can reach Sink t
vector<bool> get_t_reachable(int t, int n) {
    // 1. Build the Transpose Graph (Reverse Edges)
    // rev_adj[v] will contain 'u' if there is a valid edge u -> v
    vector<vector<int>> rev_adj(n + 1); // +1 just in case of 1-based indexing
    
    for(int u = 0; u < n; u++) {
        for(auto& e : adj[u]) {
            // IF the forward edge (u -> e.to) has remaining capacity...
            if(e.capacity - e.flow > 0) {
                // ...THEN we can travel backwards from e.to -> u
                rev_adj[e.to].push_back(u);
            }
        }
    }

    // 2. Standard BFS starting from Sink (t)
    vector<bool> visited(n + 1, false);
    queue<int> q;

    q.push(t);
    visited[t] = true;

    while(!q.empty()) {
        int u = q.front(); 
        q.pop();

        for(int v : rev_adj[u]) {
            if(!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }
    return visited;
}


void solve(int case_num) {
    // ... (Build Graph and Run Max Flow first) ...
    edmond_karp(S, T, MAX_NODES);

    // Get Reachability Arrays
    vector<bool> can_reach_T = get_t_reachable(T, MAX_NODES);
    
    // Example Usage: Check if edge u->v bridges the gap
    // If (S can reach u) AND (v can reach T), then u->v is a critical edge.
    if (can_reach_S[u] && can_reach_T[v]) {
        // This edge helps increase flow!
    }
}