#include <bits/stdc++.h>
using namespace std;

// ... (Include your standard Edge struct, bfs, and edmond_karp functions here) ...

int main() {
    int N, M, K; // N total nodes, K edges
    // Usually input gives size of Set A and Set B
    int nA, nB; 
    cin >> nA >> nB >> K; 
    
    int total_nodes = nA + nB;
    int S = 0, T = total_nodes + 1;
    
    // Adjust adjacency list size
    // Nodes 1..nA are Left side
    // Nodes nA+1..nA+nB are Right side
    adj.assign(T + 2, vector<Edge>());

    // 1. Build Bipartite Graph
    for (int i = 0; i < K; i++) {
        int u, v;
        cin >> u >> v; // u from Set A, v from Set B
        
        // Connect Left u -> Right v
        // (Assuming input u, v are 1-based. Adjust if 0-based)
        int right_v = nA + v; 
        
        int uidx = adj[u].size();
        int vidx = adj[right_v].size();
        adj[u].push_back({right_v, 1, 0, vidx}); // Directed u -> v
        adj[right_v].push_back({u, 0, 0, uidx});
    }

    // 2. Connect Source to Left Side
    for (int i = 1; i <= nA; i++) {
        int sidx = adj[S].size();
        int iidx = adj[i].size();
        adj[S].push_back({i, 1, 0, iidx});
        adj[i].push_back({S, 0, 0, sidx});
    }

    // 3. Connect Right Side to Sink
    for (int i = 1; i <= nB; i++) {
        int u = nA + i;
        int uidx = adj[u].size();
        int tidx = adj[T].size();
        adj[u].push_back({T, 1, 0, tidx});
        adj[T].push_back({u, 0, 0, uidx});
    }

    // 4. Run Max Flow (Calculate Max Matching)
    int max_matching = edmond_karp(S, T, T + 2);

    // 5. The Magic Formula
    // Max Independent Set = Total Vertices - Max Matching
    cout << total_nodes - max_matching << endl;

    return 0;
}