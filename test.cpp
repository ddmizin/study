#include <iostream>
#include <vector>
#include <queue>

struct Edge {
    int to;
    int w;
};

int main() {
    int N, M;
    std::cin >> N >> M;

    int S, F;
    std::cin >> S >> F;

    std::vector<std::vector<Edge>> g(N + 1);

    for (int i = 0; i < M; i++) {
        int b, e, w;
        std::cin >> b >> e >> w;

        g[b].push_back({e, w});
        g[e].push_back({b, w});
    }

    const int INF = 1000000000;
    std::vector<int> dist(N + 1, INF);

    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<std::pair<int,int>>> pq;

    dist[S] = 0;
    pq.push({0, S});

    while (!pq.empty()) {

        auto [d, v] = pq.top();
        pq.pop();

        if (d != dist[v]) continue;

        if (v == F) break;

        for (const Edge &e : g[v]) {

            int to = e.to;
            int nd = d + e.w;

            if (nd < dist[to]) {
                dist[to] = nd;
                pq.push({nd, to});
            }
        }
    }

    if (dist[F] == INF)
        std::cout << -1;
    else
        std::cout << dist[F];
}