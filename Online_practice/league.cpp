void solve_league() {
    int N, g;
    cin >> N >> g;
    // ... Read teams, cities, ranks ...
    
    // Separate into Odd and Even ranked teams
    vector<int> odds, evens; 
    // ... fill vectors with Team IDs ...

    int S = 0, T = N + 1;
    
    // 1. Source -> Odd Teams (Cap g)
    for(int u : odds) add_edge(S, u, g);

    // 2. Even Teams -> Sink (Cap g)
    for(int v : evens) add_edge(v, T, g);

    // 3. Odd -> Even (Cap 1, if different city)
    for(int u : odds) {
        for(int v : evens) {
            if(team_city[u] != team_city[v]) {
                add_edge(u, v, 1);
            }
        }
    }

    if (edmond_karp(S, T, N + 5) == odds.size() * g) 
        cout << "Yes" << endl;
    else 
        cout << "No" << endl;
}