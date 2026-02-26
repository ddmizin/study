#include <iostream>
#include <vector>

std::vector<std::vector<int>> graph;
std::vector<bool> visited;

void dfs(int v) {
    visited[v] = true;
    for (int to : graph[v]) {
        if (!visited[to])
            dfs(to);
    }
}

int main() {
    int n, m;
    std::cin >> n >> m;

    if (n < 3 || m != n) {
        std::cout << "EUCLID\n";
        return 0;
    }

    graph.assign(n + 1, {});
    visited.assign(n + 1, false);

    for (int i = 0; i < m; i++) {
        int u, v;
        std::cin >> u >> v;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    dfs(1);

    for (int i = 1; i <= n; i++) {
        if (!visited[i]) {
            std::cout << "EUCLID\n";
            return 0;
        }
    }

    std::cout << "ARCHIMEDES\n";
}