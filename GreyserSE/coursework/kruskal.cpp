#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

struct Edge {
    int from;
    int to;
    int weight;

    bool operator<(const Edge& other) const {
        return weight < other.weight;
    }
};

class DisjointSetUnion {
private:
    vector<int> parent;
    vector<int> rank;

public:
    explicit DisjointSetUnion(int n) : parent(n), rank(n, 0) {
        for (int i = 0; i < n; ++i)
            parent[i] = i;
    }

    int find(int v) {
        if (v != parent[v])
            parent[v] = find(parent[v]);
        return parent[v];
    }

    void unite(int u, int v) {
        u = find(u);
        v = find(v);
        if (u == v) return;
        if (rank[u] < rank[v])
            parent[u] = v;
        else {
            parent[v] = u;
            if (rank[u] == rank[v])
                ++rank[u];
        }
    }
};


vector<Edge> load_edges_from_file(const string& filename, int& vertex_count) {
    ifstream in(filename);
    int edge_count;
    in >> vertex_count >> edge_count;

    vector<Edge> edges(edge_count);
    for (int i = 0; i < edge_count; ++i) {
        in >> edges[i].from >> edges[i].to >> edges[i].weight;
    }
    return edges;
}


pair<int, vector<Edge>> kruskal_mst(int vertex_count, const vector<Edge>& edges) {
    DisjointSetUnion dsu(vertex_count);
    vector<Edge> mst;
    int total_weight = 0;

    vector<Edge> sorted_edges = edges;
    sort(sorted_edges.begin(), sorted_edges.end());

    for (const Edge& edge : sorted_edges) {
        if (dsu.find(edge.from) != dsu.find(edge.to)) {
            dsu.unite(edge.from, edge.to);
            mst.push_back(edge);
            total_weight += edge.weight;
            if (mst.size() == vertex_count - 1) break;
        }
    }

    return {total_weight, mst};
}

int main() {
    string filename = "C://C_programming//coursework//graph1000_thin.txt";
    int vertex_count;
    vector<Edge> edges = load_edges_from_file(filename, vertex_count);

    auto mst = kruskal_mst(vertex_count, edges);

    cout << "Минимальное остовное дерево построено.\n";
    cout << "Количество рёбер в MST: " << mst.second.size() << "\n";
    cout << "Суммарный вес MST: " << mst.first << "\n";

    return 0;
}

