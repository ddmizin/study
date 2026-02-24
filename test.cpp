#include <iostream>
#include <vector>

std::vector<std::vector<int>> graph;
std::vector<bool> visited;
bool hasCycle = false;

void dfs(int v, int parent) {
    visited[v] = true;

    for (int to : graph[v]) {
        if (!visited[to]) {
            dfs(to, v);
        }
        else if (to != parent) {
            hasCycle = true;
        }
    }
}

int main() {
    int N, M;
    std::cin >> N >> M;

    graph.resize(N + 1);
    visited.assign(N + 1, false);

    for (int i = 0; i < M; i++) {
        int u, v;
        std::cin >> u >> v;
        graph[u].push_back(v);
        graph[v].push_back(u);
    }

    for (int i = 1; i <= N && !hasCycle; i++) {
        if (!visited[i]) {
            dfs(i, -1);
        }
    }

    std::cout << (hasCycle ? "YES\n" : "NO\n");
}