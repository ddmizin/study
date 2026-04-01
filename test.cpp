#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const long long INF = 2e18;

struct Edge {
    int to;
    long long cap;
    long long flow;
    int rev;
};

vector<vector<Edge>> adj;

void add_edge(int from, int to, long long cap) {
    adj[from].push_back({to, cap, 0, (int)adj[to].size()});
    adj[to].push_back({from, 0, 0, (int)adj[from].size() - 1});
}


long long edmonds_karp(int s, int t) {
    long long flow = 0;
    vector<int> parent_node(adj.size());
    vector<int> parent_edge(adj.size());

    while (true) {
        fill(parent_node.begin(), parent_node.end(), -1);
        parent_node[s] = -2;
        queue<pair<int, long long>> q;
        q.push({s, INF});

        long long pushed = 0;

        while (!q.empty()) {
            int v = q.front().first;
            long long cur_flow = q.front().second;
            q.pop();

            for (int i = 0; i < adj[v].size(); ++i) {
                auto& edge = adj[v][i];
                if (parent_node[edge.to] == -1 && edge.cap - edge.flow > 0) {
                    parent_node[edge.to] = v;
                    parent_edge[edge.to] = i;
                    long long new_flow = min(cur_flow, edge.cap - edge.flow);
                    if (edge.to == t) {
                        pushed = new_flow;
                        break;
                    }
                    q.push({edge.to, new_flow});
                }
            }
            if (pushed) break;
        }

        if (pushed == 0) break;

        flow += pushed;
        int curr = t;
        while (curr != s) {
            int p = parent_node[curr];
            int idx = parent_edge[curr];
            int rev_idx = adj[p][idx].rev;

            adj[p][idx].flow += pushed;
            adj[curr][rev_idx].flow -= pushed;
            curr = p;
        }
    }
    return flow;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m, p;
    if (!(cin >> n >> m >> p)) return 0;

    int S = 0;
    int T = n + m + p + 1;

    adj.assign(T + 1, vector<Edge>());

    long long total_V = 0;


    for (int i = 1; i <= n; ++i) {
        long long v;
        int k;
        cin >> v >> k;
        total_V += v;
        
        add_edge(S, i, v);
        
        for (int j = 0; j < k; ++j) {
            int mod_idx;
            cin >> mod_idx;
            add_edge(i, n + mod_idx, INF); 
        }
    }

    vector<long long> C(m + 1);
    for (int j = 1; j <= m; ++j) {
        cin >> C[j];
    }

    vector<bool> in_pair(m + 1, false);

    for (int idx = 1; idx <= p; ++idx) {
        int a, b;
        long long d;
        cin >> a >> b >> d;
        in_pair[a] = true;
        in_pair[b] = true;

        C[a] = min(C[a], d);
        C[b] = min(C[b], d);

        long long delta = C[a] + C[b] - d;

        add_edge(n + a, T, d - C[b]);
        add_edge(n + b, T, d - C[a]);

        int aux = n + m + idx;
        add_edge(aux, T, delta);
        add_edge(n + a, aux, INF); 
        add_edge(n + b, aux, INF); 
    }


    for (int j = 1; j <= m; ++j) {
        if (!in_pair[j]) {
            add_edge(n + j, T, C[j]);
        }
    }

    long long max_profit = total_V - edmonds_karp(S, T);
    cout << max_profit << "\n";

    return 0;
}
