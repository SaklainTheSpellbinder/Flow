#include <bits/stdc++.h>
using namespace std;

#define pb push_back
#define ll long long
#define FAST_IO ios_base::sync_with_stdio(0); cin.tie(0);
#define nl "\n"

// --- YOUR TEMPLATE ---
struct Edge{ int to, capacity, flow, rev; };
vector<vector<Edge>> adj;
vector<int> par_node, par_edge_idx;

bool bfs(int s, int t, int n){
    par_node.assign(n, -1); par_edge_idx.assign(n, -1);
    queue<int> q; q.push(s); par_node[s]=s;
    while(!q.empty()){
        int u=q.front(); q.pop();
        for(int i=0; i<adj[u].size(); i++){
            Edge& e=adj[u][i];
            if(par_node[e.to]==-1 && e.capacity-e.flow>0){
                par_node[e.to]=u; par_edge_idx[e.to]=i; q.push(e.to);
                if(e.to==t) return true;
            }
        }
    }
    return false;
}

ll edmond_karp(int s, int t, int n){
    ll max_flow=0;
    while(bfs(s,t,n)){
        int bottleneck=1e9;
        for(int v=t; v!=s; v=par_node[v]){
            int u=par_node[v]; int idx=par_edge_idx[v];
            bottleneck=min(bottleneck, adj[u][idx].capacity-adj[u][idx].flow);
        }
        for(int v=t; v!=s; v=par_node[v]){
            int u=par_node[v]; int idx=par_edge_idx[v]; int ridx=adj[u][idx].rev;
            adj[u][idx].flow+=bottleneck; adj[v][ridx].flow-=bottleneck;
        }
        max_flow+=(ll)bottleneck;
    }
    return max_flow;
}

void add_edge(int u, int v, int cap) {
    int uidx=adj[u].size(); int vidx=adj[v].size();
    adj[u].pb({v, cap, 0, vidx}); adj[v].pb({u, 0, 0, uidx});
}

// --- PROBLEM SPECIFIC ---
struct Person {
    int height, age, divorced;
};

void solve(int case_num) {
    int m, n; 
    cin >> m >> n; // m men, n women

    vector<Person> men(m), women(n);

    // Read Men
    for(int i=0; i<m; i++) {
        cin >> men[i].height >> men[i].age >> men[i].divorced;
    }
    // Read Women
    for(int i=0; i<n; i++) {
        cin >> women[i].height >> women[i].age >> women[i].divorced;
    }

    // Nodes:
    // Source = 0
    // Men = 1 to m
    // Women = m+1 to m+n
    // Sink = m+n+1
    int S = 0;
    int T = m + n + 1;
    int MAX_NODES = T + 5;
    
    adj.assign(MAX_NODES, vector<Edge>());

    // 1. Source -> Men
    for(int i=0; i<m; i++) {
        add_edge(S, i+1, 1);
    }

    // 2. Women -> Sink
    for(int i=0; i<n; i++) {
        add_edge(m + i + 1, T, 1);
    }

    // 3. Men -> Women (Matching Logic)
    for(int i=0; i<m; i++) {
        for(int j=0; j<n; j++) {
            // Check Conditions
            bool h_ok = abs(men[i].height - women[j].height) <= 12;
            bool a_ok = abs(men[i].age - women[j].age) <= 5;
            bool d_ok = (men[i].divorced == women[j].divorced);

            if(h_ok && a_ok && d_ok) {
                // Add edge from Man (i+1) to Woman (m+j+1)
                add_edge(i+1, m + j + 1, 1);
            }
        }
    }

    cout << "Case " << case_num << ": " << edmond_karp(S, T, MAX_NODES) << nl;
}

int main() {
    FAST_IO;
    int t; 
    if(cin >> t) {
        for(int i=1; i<=t; i++) solve(i);
    }
    return 0;
}