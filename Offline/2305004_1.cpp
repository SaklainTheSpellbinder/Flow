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


void print_min_cut(int s, int n) {
    // 1. Find reachable nodes in residual graph
    vector<bool> reachable(n, false);
    queue<int> q;
    q.push(s);
    reachable[s] = true;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (auto& e : adj[u]) {
            // If edge has remaining capacity, we can cross it
            if (e.capacity - e.flow > 0 && !reachable[e.to]) {
                reachable[e.to] = true;
                q.push(e.to);
            }
        }
    }

    // 2. Identify Cut Edges
    cout << "Min-Cut Edges:" << endl;
    for (int u = 0; u < n; u++) {
        if (reachable[u]) { // u is on Source side
            for (auto& e : adj[u]) {
                if (!reachable[e.to] && e.capacity > 0) { // v is on Sink side
                    // Note: Check e.capacity > 0 to ignore back-edges
                    cout << u << " -> " << e.to << endl;
                }
            }
        }
    }
}

int main() {
    FAST_IO;
    int N,M;
    cin>>N>>M;
    adj.assign(N,vector<Edge>());
    vector<pair<int,int>>edges;
    for(int i=0;i<M;i++){
        int u,v,c;
        cin>>u>>v>>c;
        int uidx=adj[u].size();
        int vidx=adj[v].size();
        adj[u].push_back({v,c,0,vidx});
        adj[v].push_back({u,0,0,uidx});
        edges.push_back({u,uidx});
    }
    int s,t;
    cin>>s>>t;
    ll total_flow=edmond_karp(s,t,N);
    cout<<total_flow<<endl;
    for(int i=0;i<M;i++){
        int u=edges[i].first;
        int idx=edges[i].second;
        cout<<u<<" "<<adj[u][idx].to<<" "<<adj[u][idx].flow<<"/"<<adj[u][idx].capacity<<endl;
    }
    return 0;
}