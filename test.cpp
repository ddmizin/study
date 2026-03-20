#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>


int main() {
    long long h;
    std::cin >> h;

    int a, b, c;
    std::cin >> a >> b >> c;

    int m = std::min({a, b, c});

    std::vector<int> steps = {a, b, c};

    const long long INF = 4e18;
    std::vector<long long> dist(m, INF);

    int start = 1 % m;
    dist[start] = 1;

    std::priority_queue<std::pair<long long,int>, std::vector<std::pair<long long,int>>, std::greater<>> pq;

    pq.push({1, start});

    while (!pq.empty()) {
        auto [d, r] = pq.top();
        pq.pop();

        if (d != dist[r]) continue;

        for (int step : steps) {
            int nr = (r + step) % m;
            long long nd = d + step;

            if (nd < dist[nr]) {
                dist[nr] = nd;
                pq.push({nd, nr});
            }
        }
    }

    long long ans = 0;

    for (int r = 0; r < m; r++) {
        if (dist[r] <= h) {
            ans += (h - dist[r]) / m + 1;
        }
    }

    std::cout << ans;
}