int main() {
    // ... (Setup balances, SS, TT, Feedback Edge t->s same as before) ...

    // --- PHASE 1: Feasibility ---
    ll flow1 = edmond_karp(SS, TT, N + 2);

    if (flow1 != total_supply) {
        cout << "Impossible" << endl;
        return 0;
    }

    // Capture the valid flow we found initially
    // This flow satisfies all lower bounds, but it might be too large.
    int t_idx = /* index of t->s edge */;
    ll base_flow = adj[t][t_idx].flow;

    // --- PHASE 2: Minimization ---
    
    // 1. Remove Feedback Edge t -> s
    adj[t][t_idx].capacity = 0;
    adj[t][t_idx].flow = 0;
    
    // Remove its reverse part (s -> t)
    int s_rev = adj[t][t_idx].rev;
    adj[s][s_rev].capacity = 0;
    adj[s][s_rev].flow = 0;

    // 2. Run Max Flow BACKWARDS (t -> s)
    // We try to push flow back to S to cancel out the forward flow.
    ll can_cancel_amount = edmond_karp(t, s, N + 2);

    cout << base_flow - can_cancel_amount << endl;
    
    return 0;
}