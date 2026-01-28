int main() {
    int N, M; // N projects, M dependencies
    cin >> N >> M;

    int S = 0, T = N + 1;
    adj.assign(N + 2, vector<Edge>());

    ll total_possible_profit = 0;

    // 1. Read Project Values (Profits/Costs)
    vector<int> values(N + 1);
    for (int i = 1; i <= N; i++) {
        cin >> values[i];

        if (values[i] > 0) {
            // Profitable Project: Connect Source -> i
            total_possible_profit += values[i];
            
            int sidx = adj[S].size();
            int iidx = adj[i].size();
            adj[S].push_back({i, values[i], 0, iidx});
            adj[i].push_back({S, 0, 0, sidx});
        } 
        else if (values[i] < 0) {
            // Costly Project: Connect i -> Sink
            // Note: Use POSITIVE capacity (absolute value)
            int cost = -values[i]; 
            
            int iidx = adj[i].size();
            int tidx = adj[T].size();
            adj[i].push_back({T, cost, 0, tidx});
            adj[T].push_back({i, 0, 0, iidx});
        }
        // If values[i] == 0, we don't need to connect it to S or T, 
        // but it still might be part of a dependency chain.
    }

    // 2. Read Dependencies
    // Input format: "u v" means Project u REQUIRES Project v
    for (int i = 0; i < M; i++) {
        int u, v;
        cin >> u >> v;

        // Dependency Edge: u -> v with INFINITE capacity
        int uidx = adj[u].size();
        int vidx = adj[v].size();
        adj[u].push_back({v, (int)1e9, 0, vidx});
        adj[v].push_back({u, 0, 0, uidx});
    }

    // 3. Calculate Min Cut (Max Flow)
    ll min_loss = edmond_karp(S, T, N + 2);

    // 4. Final Result
    cout << "Maximum Profit: " << total_possible_profit - min_loss << endl;

    return 0;
}