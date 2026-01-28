int main() {
    FAST_IO;
    int T_cases;
    cin >> T_cases;
    for (int cs = 1; cs <= T_cases; cs++) {
        int N;
        cin >> N; // Number of regulators

        // 1. Read Node Capacities
        vector<int> node_cap(N + 1);
        for (int i = 1; i <= N; i++) {
            cin >> node_cap[i];
        }

        int M;
        cin >> M; // Number of cables (edges)

        // Reset Adjacency List
        // Size: 2*N (for split nodes) + 2 (for Super S/T) + buffer
        int S = 0;
        int T = 2 * N + 1;
        adj.assign(T + 5, vector<Edge>());

        // 2. Build Internal Node Edges (Node Splitting)
        // Edge: u_in (i) -> u_out (i+N)
        for (int i = 1; i <= N; i++) {
            int u = i;
            int v = i + N;
            int c = node_cap[i];
            
            // Add edge u -> v
            int uidx = adj[u].size();
            int vidx = adj[v].size();
            adj[u].push_back({v, c, 0, vidx});
            adj[v].push_back({u, 0, 0, uidx});
        }

        // 3. Build Cable Edges
        // Input: u v c
        // Logic: u_out (u+N) -> v_in (v)
        for (int i = 0; i < M; i++) {
            int u, v, c;
            cin >> u >> v >> c;
            
            int u_out = u + N;
            int v_in = v;

            int uidx = adj[u_out].size();
            int vidx = adj[v_in].size();
            adj[u_out].push_back({v_in, c, 0, vidx});
            adj[v_in].push_back({u_out, 0, 0, uidx});
        }

        // 4. Handle Sources and Destinations
        int B, D;
        cin >> B >> D;
        
        // Super-Source -> Actual Sources (u_in)
        for(int i=0; i<B; i++) {
            int start_node;
            cin >> start_node;
            
            // Edge: S -> start_node
            int uidx = adj[S].size();
            int vidx = adj[start_node].size();
            adj[S].push_back({start_node, (int)1e9, 0, vidx}); // Infinite capacity
            adj[start_node].push_back({S, 0, 0, uidx});
        }

        // Actual Destinations (v_out) -> Super-Sink
        for(int i=0; i<D; i++) {
            int end_node;
            cin >> end_node;
            
            int u_out = end_node + N;
            
            // Edge: end_node_out -> T
            int uidx = adj[u_out].size();
            int vidx = adj[T].size();
            adj[u_out].push_back({T, (int)1e9, 0, vidx}); // Infinite capacity
            adj[T].push_back({u_out, 0, 0, uidx});
        }

        // 5. Run Max Flow
        cout << "Case " << cs << ": " << edmond_karp(S, T, T + 1) << endl;
    }
    return 0;
}