#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <algorithm>

using namespace std;

struct DSU {
    vector<int> parent;
    DSU(int n) {
        parent.resize(n);
        iota(parent.begin(), parent.end(), 0);
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    bool unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            parent[root_i] = root_j;
            return true;
        }
        return false;
    }
};

struct Point {
    long long x, y;
};

struct Edge {
    int u, v;
};

struct Pairing {
    int e[2][2];
    double cost;
    bool operator<(const Pairing& other) const {
        return cost < other.cost;
    }
};

struct MetaEdge {
    int u, v;
    double w;
    bool operator<(const MetaEdge& other) const {
        return w < other.w;
    }
};

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int N, M;
    if (!(cin >> N >> M)) return 0;

    vector<Point> coords(N);
    for (int i = 0; i < N; ++i) {
        cin >> coords[i].x >> coords[i].y;
    }

    vector<Edge> edges(M);
    vector<vector<int>> adj(N);
    for (int i = 0; i < M; ++i) {
        cin >> edges[i].u >> edges[i].v;
        adj[edges[i].u].push_back(i);
        adj[edges[i].v].push_back(i);
    }

    auto get_vec = [&](int edge_id, int v_id) -> pair<long long, long long> {
        int u = (edges[edge_id].u == v_id) ? edges[edge_id].v : edges[edge_id].u;
        return {coords[u].x - coords[v_id].x, coords[u].y - coords[v_id].y};
    };

    auto get_angle = [&](int e1, int e2, int v_id) -> double {
        auto vec1 = get_vec(e1, v_id);
        auto vec2 = get_vec(e2, v_id);
        long long dot = vec1.first * vec2.first + vec1.second * vec2.second;
        long long cross = vec1.first * vec2.second - vec1.second * vec2.first;
        return atan2((double)std::abs(cross), (double)dot);
    };

    DSU dsu(M);
    double total_cost = 0.0;
    vector<MetaEdge> potential_meta_edges;

    for (int i = 0; i < N; ++i) {
        if (adj[i].size() == 2) {
            int e1 = adj[i][0];
            int e2 = adj[i][1];
            total_cost += get_angle(e1, e2, i);
            dsu.unite(e1, e2);
        } 
        else if (adj[i].size() == 4) {
            int e0 = adj[i][0], e1 = adj[i][1], e2 = adj[i][2], e3 = adj[i][3];
            
            Pairing p[3];
            p[0] = {{{e0, e1}, {e2, e3}}, get_angle(e0, e1, i) + get_angle(e2, e3, i)};
            p[1] = {{{e0, e2}, {e1, e3}}, get_angle(e0, e2, i) + get_angle(e1, e3, i)};
            p[2] = {{{e0, e3}, {e1, e2}}, get_angle(e0, e3, i) + get_angle(e1, e2, i)};
            
            sort(p, p + 3);

            total_cost += p[0].cost;
            dsu.unite(p[0].e[0][0], p[0].e[0][1]);
            dsu.unite(p[0].e[1][0], p[0].e[1][1]);

            double weight = p[1].cost - p[0].cost;
            potential_meta_edges.push_back({p[0].e[0][0], p[0].e[1][0], weight});
        }
    }

    vector<MetaEdge> mst_edges;
    for (const auto& me : potential_meta_edges) {
        int comp1 = dsu.find(me.u);
        int comp2 = dsu.find(me.v);
        if (comp1 != comp2) {
            mst_edges.push_back({comp1, comp2, me.w});
        }
    }

    sort(mst_edges.begin(), mst_edges.end());


    for (const auto& edge : mst_edges) {
        if (dsu.unite(edge.u, edge.v)) {
            total_cost += edge.w;
        }
    }

    cout << fixed << setprecision(9) << total_cost << "\n";

    return 0;
}