// ---------------------------------------------------------
// STEP 1: Run BFS on Residual Graph to find "Reachable Set"
// ---------------------------------------------------------
vector<bool> reachable(2 * N + 2, false); // Size based on split graph
queue<int> q;

q.push(S); 
reachable[S] = true;

while (!q.empty()) {
    int u = q.front(); 
    q.pop();

    for (auto &e : adj[u]) {
        // If edge has remaining capacity (is not full), we can cross it
        if (!reachable[e.to] && e.capacity - e.flow > 0) {
            reachable[e.to] = true;
            q.push(e.to);
        }
    }
}

// ---------------------------------------------------------
// STEP 2: Check for Destroyed NODES
// ---------------------------------------------------------
// Logic: If we can reach u_in, but cannot cross the internal 
//        edge to reach u_out, then the Node u is the bottleneck.

cout << "Nodes to Destroy:" << endl;
for (int i = 1; i <= N; i++) { // Assuming nodes 1 to N
    int u_in = i;
    int u_out = i + N;

    // Check Internal Edge: u_in -> u_out
    if (reachable[u_in] && !reachable[u_out]) {
        cout << "Node " << i << endl;
    }
}

// ---------------------------------------------------------
// STEP 3: Check for Destroyed EDGES
// ---------------------------------------------------------
// Logic: If we can reach u_out, but cannot cross the external 
//        edge to reach v_in, then the Edge u-v is the bottleneck.

cout << "Edges to Destroy:" << endl;
for (int u = 1; u <= N; u++) {
    int u_out = u + N;
    
    // Iterate through adjacency list of u_out
    // These edges connect to some v_in
    if (reachable[u_out]) { // Optimization: Only check if start is reachable
        for (auto &e : adj[u_out]) {
            int v_in = e.to; 
            
            // Note: We only care about forward edges to other nodes
            // (Exclude the reverse flow edges or internal edges)
            if (v_in <= N) { // Simple check if it points to an 'input' node
                if (!reachable[v_in]) {
                    // Start reachable, End not reachable => CUT
                    cout << "Edge " << u << " -> " << v_in << endl;
                }
            }
        }
    }
}