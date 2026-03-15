#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>
#include <climits>
#include <string>

using namespace std;

struct AdjEdge {
    int v;
    int w;
    AdjEdge(int to, int weight) : v(to), w(weight) {}
};

struct EdgeInfo {
    int a, b, w;
    EdgeInfo(int u, int v, int weight) : a(u), b(v), w(weight) {}

    bool operator<(const EdgeInfo& other) const {
        return w < other.w;
    }
};

class GraphEU {
private:

    int vertices;
    vector<vector<AdjEdge>> adj;
    vector<EdgeInfo> allEdges;

    // ---------------- shortest paths ----------------
    vector<int> shortestPaths(int start) {

        vector<int> dist(vertices, INT_MAX);
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;

        dist[start] = 0;
        pq.push({0,start});

        while(!pq.empty()) {

            auto [d,u] = pq.top();
            pq.pop();

            if(d > dist[u]) continue;

            for(const auto &edge : adj[u]) {

                int to = edge.v;

                if(dist[u] + 1 < dist[to]) {
                    dist[to] = dist[u] + 1;
                    pq.push({dist[to], to});
                }
            }
        }

        return dist;
    }

    // ---------------- greedy coloring ----------------
    int greedyColor(vector<int>& col) {

        queue<int> q;
        q.push(0);
        col[0] = 0;

        int maxColor = 0;

        while(!q.empty()) {

            int v = q.front();
            q.pop();

            for(auto &e : adj[v]) {

                int to = e.v;

                if(col[to] == -1) {

                    vector<bool> used(vertices,false);

                    for(auto &n : adj[to]) {
                        if(col[n.v] != -1)
                            used[col[n.v]] = true;
                    }

                    int c = 0;
                    while(used[c]) c++;

                    col[to] = c;
                    maxColor = max(maxColor, c);

                    q.push(to);
                }
            }
        }

        return maxColor + 1;
    }

    // ---------------- binary DFS ----------------
    void encodeDFS(int v, int parent,
                   const vector<vector<int>>& tree,
                   string& code) {

        for(int to : tree[v]) {

            if(to == parent) continue;

            code += '1';
            encodeDFS(to, v, tree, code);
            code += '0';
        }
    }

    string countryName(int id) {

        static vector<string> names = {
            "Por","Spa","Fra","Bel","Net","Lux","Ger","Aus","Ita",
            "Slovenia","Cro","Hun","Rom","Bul","Gre",
            "Slo","Cze","Pol","Den","Lit","Lat","Est"
        };

        if(id < names.size()) return names[id];
        return "Unknown";
    }

public:

    GraphEU(int n) : vertices(n) {
        adj.resize(n);
    }

    void addBorder(int a,int b,int dist) {

        adj[a].push_back(AdjEdge(b,dist));
        adj[b].push_back(AdjEdge(a,dist));

        allEdges.push_back(EdgeInfo(a,b,dist));
    }

    // ---------------- radius diameter center ----------------
    void analyzeGraph() {

        vector<int> ecc(vertices,0);

        auto check = shortestPaths(0);

        for(int d : check)
            if(d == INT_MAX){
                cout<<"Graph not connected\n";
                return;
            }

        // Вывод количества вершин и ребер
        cout << "|V| = " << vertices << endl;
        cout << "|E| = " << allEdges.size() << endl;

        for(int i=0;i<vertices;i++){

            auto dist = shortestPaths(i);

            int m = 0;

            for(int j=0;j<vertices;j++)
                if(dist[j] != INT_MAX)
                    m = max(m, dist[j]);

            ecc[i] = m;
        }

        int radius = INT_MAX;
        int diameter = 0;

        for(int e : ecc){
            radius = min(radius,e);
            diameter = max(diameter,e);
        }

        cout<<"Radius = "<<radius<<endl;
        cout<<"Diameter = "<<diameter<<endl;

        cout<<"Centers:\n";

        for(int i=0;i<vertices;i++)
            if(ecc[i]==radius)
                cout<<"   "<<countryName(i)<<endl;
    }

    // ---------------- chromatic number ----------------
    int chromaticNumber() {

        vector<int> color(vertices,-1);

        int res = greedyColor(color);

        cout<<"Chromatic number = "<<res<<endl;

        return res;
    }

    // ---------------- DSU ----------------
    class DSU{

        vector<int> parent;
        vector<int> rank;

    public:

        DSU(int n){
            parent.resize(n);
            rank.resize(n,0);

            for(int i=0;i<n;i++)
                parent[i]=i;
        }

        int findSet(int v){
            if(parent[v]==v) return v;
            return parent[v]=findSet(parent[v]);
        }

        void unite(int a,int b){

            a=findSet(a);
            b=findSet(b);

            if(a!=b){

                if(rank[a]<rank[b])
                    swap(a,b);

                parent[b]=a;

                if(rank[a]==rank[b])
                    rank[a]++;
            }
        }
    };

    // ---------------- MST ----------------
    vector<EdgeInfo> buildMST() {

        vector<EdgeInfo> edges = allEdges;

        sort(edges.begin(), edges.end());

        DSU dsu(vertices);

        vector<EdgeInfo> mst;

        for(auto &e : edges){

            if(dsu.findSet(e.a) != dsu.findSet(e.b)){

                dsu.unite(e.a,e.b);
                mst.push_back(e);
            }
        }

        cout<<"MST edges:\n";

        for(auto &e : mst){
            cout<<"   "
                <<e.a
                <<" - "
                <<e.b
                <<"  ("
                <<e.w
                <<")\n";
        }

        return mst;
    }

    // ---------------- Prüfer code ----------------
    string pruefer(const vector<EdgeInfo>& tree){

        int n = vertices;

        vector<vector<int>> g(n);

        for(auto &e : tree){
            g[e.a].push_back(e.b);
            g[e.b].push_back(e.a);
        }

        vector<int> degree(n);

        for(int i=0;i<n;i++)
            degree[i]=g[i].size();

        set<int> leaves;

        for(int i=0;i<n;i++)
            if(degree[i]==1)
                leaves.insert(i);

        vector<int> code;

        for(int i=0;i<n-2;i++){

            int leaf=*leaves.begin();
            leaves.erase(leaves.begin());

            int neighbor;

            for(int v:g[leaf])
                if(degree[v]>0){
                    neighbor=v;
                    break;
                }

            code.push_back(neighbor);

            degree[leaf]--;
            degree[neighbor]--;

            if(degree[neighbor]==1)
                leaves.insert(neighbor);
        }

        string result;

        for(int x:code)
            result+=to_string(x)+"|";

        return result;
    }

    // ---------------- binary tree code ----------------
    string binaryCode(const vector<EdgeInfo>& tree){

        vector<vector<int>> g(vertices);

        for(auto &e : tree){
            g[e.a].push_back(e.b);
            g[e.b].push_back(e.a);
        }

        for(auto &v : g)
            sort(v.begin(),v.end());

        string code;

        encodeDFS(0,-1,g,code);

        return code;
    }

    void encodeTree(){

        auto mst = buildMST();

        string p = pruefer(mst);
        string b = binaryCode(mst);

        cout<<"\nPruefer code: "<<p<<endl;
        cout<<"Binary code: "<<b<<endl;
    }
};

int main(){

    GraphEU g(22);

    g.addBorder(0, 1, 500);
    g.addBorder(1, 2, 1000);
    g.addBorder(2, 3, 250);
    g.addBorder(2, 5, 300);
    g.addBorder(2, 6, 900);
    g.addBorder(2, 8, 1100);
    g.addBorder(3, 4, 150);
    g.addBorder(3, 5, 200);
    g.addBorder(3, 6, 650);
    g.addBorder(4, 6, 550);
    g.addBorder(5, 6, 600);
    g.addBorder(6, 18, 350);
    g.addBorder(6, 17, 500);
    g.addBorder(6, 16, 300);
    g.addBorder(6, 7, 500);
    g.addBorder(7, 15, 55);
    g.addBorder(7, 16, 250);
    g.addBorder(7, 11, 215);
    g.addBorder(7, 9, 250);
    g.addBorder(7, 8, 750);
    g.addBorder(8, 9, 500);
    g.addBorder(9, 10, 100);
    g.addBorder(9, 11, 400);
    g.addBorder(10, 11, 300);
    g.addBorder(11, 12, 650);
    g.addBorder(11, 15, 150);
    g.addBorder(12, 13, 300);
    g.addBorder(13, 14, 500);
    g.addBorder(15, 16, 300);
    g.addBorder(15, 17, 550);
    g.addBorder(16, 17, 500);
    g.addBorder(17, 19, 400);
    g.addBorder(19, 20, 250);
    g.addBorder(20, 21, 300);

    cout<<"Graph analysis\n\n";

    g.analyzeGraph();

    cout<<"\n";

    g.chromaticNumber();

    cout<<"\n";

    g.encodeTree();

    return 0;
}