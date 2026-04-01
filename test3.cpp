#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int INF = 1e9;


struct Edge {
    int to;
    int cap;
    int flow;
    int cost;
    int rev;
    int id;
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m, e;
    if (!(cin >> n >> m >> e)) return 0;

    vector<int> w_L(n + 1);
    for (int i = 1; i <= n; ++i) cin >> w_L[i];

    vector<int> w_R(m + 1);
    for (int i = 1; i <= m; ++i) cin >> w_R[i];

    int S = 0;
    int T = n + m + 1; 
    vector<vector<Edge>> adj(n + m + 2);

    auto add_edge = [&](int u, int v, int cap, int cost, int id) {
        adj[u].push_back({v, cap, 0, cost, (int)adj[v].size(), id});
        adj[v].push_back({u, 0, 0, -cost, (int)adj[u].size() - 1, 0});
    };

    for (int i = 1; i <= n; ++i) {
        add_edge(S, i, 1, -w_L[i], 0);
    }

    for (int j = 1; j <= m; ++j) {
        add_edge(n + j, T, 1, -w_R[j], 0);
    }
    
    for (int i = 1; i <= e; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u, n + v, 1, 0, i);
    }

    
    vector<int> pot(n + m + 2, INF);
    pot[S] = 0;
    
    
    for (auto& edge : adj[S]) {
        if (edge.cap > 0) pot[edge.to] = min(pot[edge.to], pot[S] + edge.cost);
    }
    
    for (int u = 1; u <= n; ++u) {
        if (pot[u] == INF) continue;
        for (auto& edge : adj[u]) {
            if (edge.cap > 0) pot[edge.to] = min(pot[edge.to], pot[u] + edge.cost);
        }
    }
    
    for (int v = n + 1; v <= n + m; ++v) {
        if (pot[v] == INF) continue;
        for (auto& edge : adj[v]) {
            if (edge.cap > 0 && edge.to == T) pot[T] = min(pot[T], pot[v] + edge.cost);
        }
    }

    long long max_weight = 0;

    vector<int> dist(n + m + 2);
    vector<int> parent_node(n + m + 2);
    vector<int> parent_edge(n + m + 2);
    
    while (true) {
        dist.assign(n + m + 2, INF);
        parent_node.assign(n + m + 2, -1);
        parent_edge.assign(n + m + 2, -1);
        
        
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        dist[S] = 0;
        pq.push({0, S});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue;
            if (u == T) break;

            for (int i = 0; i < adj[u].size(); ++i) {
                auto& edge = adj[u][i];
                if (edge.cap - edge.flow > 0 && pot[u] != INF && pot[edge.to] != INF) {
                    int new_cost = dist[u] + edge.cost + pot[u] - pot[edge.to];
                    if (dist[edge.to] > new_cost) {
                        dist[edge.to] = new_cost;
                        parent_node[edge.to] = u;
                        parent_edge[edge.to] = i;
                        pq.push({dist[edge.to], edge.to});
                    }
                }
            }
        }

        if (dist[T] == INF) break;
        
        if (dist[T] + pot[T] >= 0) break;

        int true_cost = dist[T] + pot[T];
        max_weight += -true_cost;

        int curr = T;
        while (curr != S) {
            int p = parent_node[curr];
            int idx = parent_edge[curr];
            int rev_idx = adj[p][idx].rev;

            adj[p][idx].flow += 1;
            adj[curr][rev_idx].flow -= 1;
            curr = p;
        }

        for (int i = 0; i <= n + m + 1; ++i) {
            if (pot[i] != INF) {
                pot[i] += min(dist[i], dist[T]);
            }
        }
    }

    vector<int> result_edges;
    for (int u = 1; u <= n; ++u) {
        for (auto& edge : adj[u]) {
            if (edge.id > 0 && edge.flow > 0) {
                result_edges.push_back(edge.id);
            }
        }
    }

    cout << max_weight << "\n";
    cout << result_edges.size() << "\n";
    for (size_t i = 0; i < result_edges.size(); ++i) {
        cout << result_edges[i] << (i + 1 == result_edges.size() ? "" : " ");
    }
    cout << "\n";

    return 0;
}