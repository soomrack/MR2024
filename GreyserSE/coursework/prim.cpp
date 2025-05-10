#include <iostream>
#include <vector>
#include <queue>
#include <fstream>
#include <stdexcept>
#include <limits>

using namespace std;

struct Edge {
    int from;
    int to;
    int weight;

    bool operator<(const Edge& other) const {
        return weight > other.weight;
    }
};

vector<Edge> load_edges_from_file(const string& filename, int& vertex_count) {
    ifstream in(filename);
    if (!in.is_open()) throw runtime_error("Невозможно открыть файл");

    int edge_count;
    in >> vertex_count >> edge_count;

    vector<Edge> edges(edge_count);
    for (int i = 0; i < edge_count; ++i) {
        in >> edges[i].from >> edges[i].to >> edges[i].weight;
    }
    return edges;
}

pair<int, vector<Edge>> prim_mst(int vertex_count, const vector<Edge>& edges) {
    vector<vector<Edge>> graph(vertex_count);

    for (const Edge& edge : edges) {
        graph[edge.from].push_back({edge.from, edge.to, edge.weight});
        graph[edge.to].push_back({edge.to, edge.from, edge.weight});
    }

    vector<bool> visited(vertex_count, false);
    priority_queue<Edge> pq;
    vector<Edge> mst;
    int total_weight = 0;

    pq.push({-1, 0, 0}); // Начинаем с вершины 0, вес 0

    while (!pq.empty()) {
        Edge edge = pq.top();
        pq.pop();

        int u = edge.to;
        if (visited[u]) continue;

        visited[u] = true;
        total_weight += edge.weight;

        if (edge.from != -1) {
            mst.push_back(edge);
        }

        for (const auto& neighbor : graph[u]) {
            if (!visited[neighbor.to]) {
                pq.push(neighbor);
            }
        }
    }

    return {total_weight, mst};
}

int main() {
    try {
        const string filename = "C://C_programming//coursework//graph1000_thin.txt";

        int vertex_count;
        vector<Edge> edges = load_edges_from_file(filename, vertex_count);

        auto mst = prim_mst(vertex_count, edges);

        cout << "Минимальное остовное дерево построено.\n";
        cout << "Количество рёбер в MST: " << mst.second.size() << "\n";
        cout << "Суммарный вес MST: " << mst.first << "\n";

    } catch (const exception& ex) {
        cerr << "Ошибка: " << ex.what() << "\n";
    }

    return 0;
}


    
