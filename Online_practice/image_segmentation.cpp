int main() {
    int H, W; // Image Height and Width
    cin >> H >> W;
    int N = H * W; // Total pixels
    
    // Pixel (r, c) maps to ID: r * W + c
    auto get_id = [&](int r, int c) { return r * W + c; };

    int S = N, T = N + 1;
    adj.assign(N + 2, vector<Edge>());

    // 1. Read Likelihoods (Data Term)
    for (int r = 0; r < H; r++) {
        for (int c = 0; c < W; c++) {
            int u = get_id(r, c);
            int fore_score, back_score;
            cin >> fore_score >> back_score;

            // Edge S -> u (Capacity = Foreground Score)
            int sidx = adj[S].size();
            int uidx = adj[u].size();
            adj[S].push_back({u, fore_score, 0, uidx});
            adj[u].push_back({S, 0, 0, sidx});

            // Edge u -> T (Capacity = Background Score)
            int tidx = adj[T].size();
            adj[u].push_back({T, back_score, 0, tidx});
            adj[T].push_back({u, 0, 0, uidx}); // uidx is updated! (be careful with indices in real code)
        }
    }

    // 2. Read Penalties (Smoothness Term)
    // Assume input gives separate penalties for horizontal/vertical neighbors
    int K; 
    cin >> K; // Number of neighbor relations defined
    for(int i=0; i<K; i++) {
        int r1, c1, r2, c2, penalty;
        cin >> r1 >> c1 >> r2 >> c2 >> penalty;
        
        int u = get_id(r1, c1);
        int v = get_id(r2, c2);

        // Undirected Edge between neighbors (Capacity = Penalty)
        // Add u -> v
        int uidx = adj[u].size();
        int vidx = adj[v].size();
        adj[u].push_back({v, penalty, 0, vidx});
        
        // Add v -> u
        adj[v].push_back({u, penalty, 0, uidx});
    }

    // 3. Solve
    ll min_loss = edmond_karp(S, T, N + 2);

    // Total Score = (Sum of ALL scores) - Min_Loss
    // NOTE: Often output is just min_loss (Min Energy), read problem carefully.
    cout << min_loss << endl;

    return 0;
}