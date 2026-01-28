#include <bits/stdc++.h>
using namespace std;

const int INF = 1e9;

// --- Edge Structure ---
struct Edge {
    int to;
    int capacity;
    int flow;
    int rev; // index of the reverse edge in adj[to]
};

vector<vector<Edge>> adj;
vector<int> parent_edge_idx;
vector<int> parent_node;

// --- Add Edge Function ---
void add_edge(int u, int v, int cap) {
    Edge a = {v, cap, 0, (int)adj[v].size()};
    Edge b = {u, 0, 0, (int)adj[u].size()}; // Reverse edge with 0 capacity
    adj[u].push_back(a);
    adj[v].push_back(b);
}

// --- BFS for Max Flow (Find Augmenting Path) ---
int bfs(int s, int t) {
    fill(parent_node.begin(), parent_node.end(), -1);
    parent_node[s] = -2;
    queue<pair<int, int>> q;
    q.push({s, INF});

    while (!q.empty()) {
        int u = q.front().first;
        int flow = q.front().second;
        q.pop();

        for (int i = 0; i < adj[u].size(); i++) {
            Edge &e = adj[u][i];
            if (parent_node[e.to] == -1 && e.capacity - e.flow > 0) {
                parent_node[e.to] = u;
                parent_edge_idx[e.to] = i;
                int new_flow = min(flow, e.capacity - e.flow);
                if (e.to == t) return new_flow;
                q.push({e.to, new_flow});
            }
        }
    }
    return 0;
}

// --- Edmonds-Karp Max Flow ---
void max_flow(int s, int t) {
    int flow = 0;
    while (int new_flow = bfs(s, t)) {
        flow += new_flow;
        int cur = t;
        while (cur != s) {
            int prev = parent_node[cur];
            int idx = parent_edge_idx[cur];
            adj[prev][idx].flow += new_flow;
            int rev_idx = adj[prev][idx].rev;
            adj[cur][rev_idx].flow -= new_flow;
            cur = prev;
        }
    }
    // We don't return flow here because we need the RESIDUAL GRAPH state
}

int main() {
    int N, M;
    if (!(cin >> N >> M)) return 0;

    // Adjust for 1-based indexing (size N + 1)
    adj.assign(N + 1, vector<Edge>());
    parent_node.resize(N + 1);
    parent_edge_idx.resize(N + 1);

    // 1. Read Existing Roads
    for (int i = 0; i < M; i++) {
        int u, v, c;
        cin >> u >> v >> c;
        add_edge(u, v, c);
    }

    int S = 1, T = N; // Problem states S=1, T=N

    // 2. Run Max Flow to saturate the current network
    max_flow(S, T);

    // 3. Find S_reachable (Nodes reachable from Source in Residual Graph)
    vector<bool> s_reach(N + 1, false);
    queue<int> q;
    q.push(S);
    s_reach[S] = true;
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto &e : adj[u]) {
            if (!s_reach[e.to] && e.capacity - e.flow > 0) {
                s_reach[e.to] = true;
                q.push(e.to);
            }
        }
    }

    // 4. Find T_reachable (Nodes that can reach Sink in Residual Graph)
    // We do BFS backwards from Sink using Reverse Edges (v->u)
    // In residual graph, flow v->u is possible if forward edge u->v has flow > 0
    // OR if reverse edge v->u has remaining capacity.
    // Simpler: Just look at incoming edges to 'u' that have capacity.
    vector<bool> t_reach(N + 1, false);
    queue<int> q2;
    q2.push(T);
    t_reach[T] = true;

    // To do backward BFS efficiently without building a reverse graph:
    // We iterate over all nodes or pre-build a transpose graph.
    // Since N is small in exams, we can do a standard BFS on a temporary 'rev_adj'.
    
    vector<vector<int>> rev_adj(N + 1);
    for (int u = 1; u <= N; u++) {
        for (auto &e : adj[u]) {
            // If we can move u -> v in residual graph...
            if (e.capacity - e.flow > 0) {
                // ...then in reverse graph, there is edge v -> u
                rev_adj[e.to].push_back(u); 
            }
        }
    }

    while(!q2.empty()){
        int u = q2.front(); q2.pop();
        for(int v : rev_adj[u]){
            if(!t_reach[v]){
                t_reach[v] = true;
                q2.push(v);
            }
        }
    }

    // 5. Process Proposals
    int P;
    cin >> P;
    
    vector<int> valid_proposals;
    for (int i = 1; i <= P; i++) {
        int u, v, c;
        cin >> u >> v >> c;

        // CRITICAL CHECK:
        // Can Source reach u? AND Can v reach Sink?
        if (s_reach[u] && t_reach[v]) {
            valid_proposals.push_back(i);
        }
    }

    // 6. Output
    if (valid_proposals.empty()) {
        cout << "None" << endl;
    } else {
        for (int i = 0; i < valid_proposals.size(); i++) {
            cout << valid_proposals[i] << (i == valid_proposals.size() - 1 ? "" : " ");
        }
        cout << endl;
    }

    return 0;
}