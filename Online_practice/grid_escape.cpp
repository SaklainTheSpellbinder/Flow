// Mapping 2D grid to 1D index
// Cell (r, c) becomes node ID: r * C + c
// Split nodes:
// In-Node:  2 * (r * C + c)
// Out-Node: 2 * (r * C + c) + 1

int get_id(int r, int c, int type) {
    // type 0 = in, type 1 = out
    return 2 * (r * M + c) + type;
}

// Inside Main Loop
for (int i = 0; i < N; i++) {
    for (int j = 0; j < M; j++) {
        int u_in = get_id(i, j, 0);
        int u_out = get_id(i, j, 1);
        
        // 1. Internal Edge (Vertex Capacity)
        addEdge(u_in, u_out, 1);

        // 2. Source Connection (If person exists)
        if (grid[i][j] == '*') {
            total_people++;
            addEdge(S, u_in, 1);
        }

        // 3. Sink Connection (If boundary)
        if (i == 0 || i == N - 1 || j == 0 || j == M - 1) {
            addEdge(u_out, T, 1);
        }

        // 4. Connect to Neighbors
        int dr[] = {0, 0, 1, -1};
        int dc[] = {1, -1, 0, 0};
        
        for (int k = 0; k < 4; k++) {
            int ni = i + dr[k];
            int nj = j + dc[k];
            
            if (ni >= 0 && ni < N && nj >= 0 && nj < M) {
                int v_in = get_id(ni, nj, 0);
                // Edge from Current OUT to Neighbor IN
                addEdge(u_out, v_in, 1);
            }
        }
    }
}