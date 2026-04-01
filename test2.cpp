#include <iostream>
#include <vector>

using namespace std;


vector<vector<int>> adjL, adjR;
vector<int> mt; 
vector<int> matchL;  
vector<bool> used;


bool try_kuhn(int v) {
    if (used[v]) return false;
    used[v] = true;
    for (int to : adjL[v]) {
        if (mt[to] == -1 || try_kuhn(mt[to])) {
            mt[to] = v;
            matchL[v] = to;
            return true;
        }
    }
    return false;
}

vector<bool> winP_L, winP_R;


void dfsL(int u) {
    winP_L[u] = true;
    for (int v : adjL[u]) {
        if (matchL[u] == v) continue;
        int next_u = mt[v];
        
        if (next_u != -1 && !winP_L[next_u]) {
            dfsL(next_u);
        }
    }
}


void dfsR(int v) {
    winP_R[v] = true; 
    for (int u : adjR[v]) {
        if (mt[v] == u) continue; 
        int next_v = matchL[u];
        
        if (next_v != -1 && !winP_R[next_v]) {
            dfsR(next_v);
        }
    }
}

int main() {
    int n1, n2, m;
    if (!(cin >> n1 >> n2 >> m)) return 0;

    adjL.resize(n1 + 1);
    adjR.resize(n2 + 1);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        adjL[u].push_back(v);
        adjR[v].push_back(u);
    }

    mt.assign(n2 + 1, -1);
    matchL.assign(n1 + 1, -1);

    
    for (int i = 1; i <= n1; ++i) {
        used.assign(n1 + 1, false);
        try_kuhn(i);
    }

    winP_L.assign(n1 + 1, false);
    
    for (int i = 1; i <= n1; ++i) {
        if (matchL[i] == -1 && !winP_L[i]) {
            dfsL(i);
        }
    }

    winP_R.assign(n2 + 1, false);
    
    for (int i = 1; i <= n2; ++i) {
        if (mt[i] == -1 && !winP_R[i]) {
            dfsR(i);
        }
    }

    
    for (int i = 1; i <= n1; ++i) {
        cout << (winP_L[i] ? 'P' : 'N');
    }
    cout << "\n";

    
    for (int i = 1; i <= n2; ++i) {
        cout << (winP_R[i] ? 'P' : 'N');
    }
    cout << "\n";

    return 0;
}