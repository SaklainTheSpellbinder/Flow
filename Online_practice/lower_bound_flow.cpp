#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define ld long double
#define mp make_pair
#define nl "\n"

#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);

struct Edge{
    int to;
    int capacity;
    int flow;
    int rev;
};

vector<vector<Edge>>adj;
vector<int>par_node;
vector<int>par_edge_idx;

bool bfs(int s,int t,int n){
    par_node.assign(n,-1);
    par_edge_idx.assign(n,-1);
    queue<int>q;

    q.push(s);
    par_node[s]=s;

    while(!q.empty()){
        int u=q.front();
        q.pop();
        for(int i=0;i<adj[u].size();i++){
            Edge& e=adj[u][i];
            if(par_node[e.to]==-1 && e.capacity-e.flow>0){
                par_node[e.to]=u;
                par_edge_idx[e.to]=i;
                q.push(e.to);
                if(e.to==t) return true;
            }
        }
    }
    return false;
}

ll edmond_karp(int s,int t,int n){
    ll max_flow=0;
    while(bfs(s,t,n)){
        int bottleneck=1e9;
        for(int v=t;v!=s;v=par_node[v]){
            int u=par_node[v];
            int idx=par_edge_idx[v];
            bottleneck=min(bottleneck,adj[u][idx].capacity-adj[u][idx].flow);
        }

        for(int v=t;v!=s;v=par_node[v]){
            int u=par_node[v];
            int idx=par_edge_idx[v];
            int ridx=adj[u][idx].rev;
            adj[u][idx].flow+=bottleneck;
            adj[v][ridx].flow-=bottleneck;
        }
        max_flow+=(ll)bottleneck;
    }
    return max_flow;
}
vector<ll>balance;

int main() {
    FAST_IO;
    int N, M;
    if (!(cin >> N >> M)) return 0;
    
    // 1. Initialize Adjacency List and Balance Array
    // N nodes (0 to N-1), plus Super-Source (N) and Super-Sink (N+1)
    int SS = N, TT = N + 1; 
    adj.assign(N + 2, vector<Edge>());
    balance.assign(N, 0);

    // 2. Read Edges with Lower Bounds
    for (int i = 0; i < M; i++) {
        int u, v, l, c;
        cin >> u >> v >> l >> c; // Read Lower Bound (l) and Capacity (c)
        
        // Add edge with capacity (C - L)
        int uidx = adj[u].size();
        int vidx = adj[v].size();
        adj[u].push_back({v, c - l, 0, vidx});
        adj[v].push_back({u, 0, 0, uidx});
        
        // Update Balances
        balance[u] -= l;
        balance[v] += l;
    }

    int s, t;
    cin >> s >> t; // Original Source and Sink

    // 3. Add Infinite Edge from t -> s to make it a Circulation
    int tidx = adj[t].size();
    int sidx = adj[s].size();
    adj[t].push_back({s, (int)1e9, 0, sidx}); // Infinite capacity
    adj[s].push_back({t, 0, 0, tidx});
    
    // Save the index of this t->s edge so we can read its flow later
    int t_to_s_edge_index = tidx; 

    // 4. Connect SS and TT based on Balance
    // Calculate total supply to verify feasibility later
    ll total_supply_needed = 0; 
    
    for(int i = 0; i < N; i++) {
        if(balance[i] > 0) {
            // Supply Node: Connect SS -> i
            total_supply_needed += balance[i];
            int ssidx = adj[SS].size();
            int iidx = adj[i].size();
            adj[SS].push_back({i, (int)balance[i], 0, iidx});
            adj[i].push_back({SS, 0, 0, ssidx});
        } 
        else if(balance[i] < 0) {
            // Demand Node: Connect i -> TT
            int iidx = adj[i].size();
            int ttidx = adj[TT].size();
            adj[i].push_back({TT, (int)-balance[i], 0, ttidx}); // Use positive value
            adj[TT].push_back({i, 0, 0, iidx});
        }
    }

    // 5. PHASE 1: Find Feasible Flow
    ll flow_phase1 = edmond_karp(SS, TT, N + 2);

    // Check Feasibility
    if (flow_phase1 != total_supply_needed) {
        cout << "Impossible" << endl; // Or "0" depending on problem
        return 0;
    }

    // 6. PHASE 2: Maximize Flow from s -> t
    // The current flow on the t->s edge is the "base" flow we satisfied
    ll current_flow = adj[t][t_to_s_edge_index].flow;

    // Effectively remove the t->s edge (set capacity to 0) so we don't use it again
    adj[t][t_to_s_edge_index].capacity = 0;
    adj[t][t_to_s_edge_index].flow = 0;
    // Also remove the reverse edge s->t (optional but clean)
    int s_rev_idx = adj[t][t_to_s_edge_index].rev;
    adj[s][s_rev_idx].capacity = 0;
    adj[s][s_rev_idx].flow = 0;

    // Run Max Flow again on the original s and t
    ll extra_flow = edmond_karp(s, t, N + 2);

    cout << current_flow + extra_flow << endl;
    
    return 0;
}