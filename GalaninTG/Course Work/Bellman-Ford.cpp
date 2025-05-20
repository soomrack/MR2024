#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <climits>
#include <algorithm>
#include <stdexcept>
#include <random>
#include <chrono>
#include <unordered_map>

using namespace std;

const int INF = INT_MAX;


struct Edge {
    string init;
    string final;
    int weight;
};


class Graph {
private:
    vector<string> vertices;
    vector<Edge> edges;
    unordered_map<string, int> vertex_to_index;

public:
    Graph(vector<string> vertices, vector<Edge> edges);
    void BellmanFord(string start);
    void FloydWarshall();
    void printGraph();
};


Graph::Graph(vector<string> A_vertices, vector<Edge> A_edges) {
    if (A_vertices.empty()) {
        throw invalid_argument("Vertex list is empty.");
    }

    set<string> unique_verts(A_vertices.begin(), A_vertices.end());
    if (unique_verts.size() != A_vertices.size()) {
        throw invalid_argument("Duplicate vertices found.");
    }

    vertices = A_vertices;
    for (size_t i = 0; i < vertices.size(); ++i) {
        vertex_to_index[vertices[i]] = i;
    }

    set<string> vert_set(vertices.begin(), vertices.end());
    for (const Edge& e : A_edges) {
        if (vert_set.find(e.init) == vert_set.end() || vert_set.find(e.final) == vert_set.end()) {
            throw invalid_argument("Edge connects non-existing vertex.");
        }
    }

    edges = A_edges;
}


void Graph::BellmanFord(string start) {
    auto it = find(vertices.begin(), vertices.end(), start);
    if (it == vertices.end()) {
        throw invalid_argument("Start vertex '" + start + "' not found.");
    }

    int v = vertices.size();
    vector<int> distances(v, INF);
    vector<int> predecessors(v, -1);
    int start_idx = vertex_to_index[start];
    distances[start_idx] = 0;

    bool changed;
    for (int i = 0; i < v - 1; ++i) {
        changed = false;
        for (const Edge& e : edges) {
            int u = vertex_to_index[e.init];
            int v_idx = vertex_to_index[e.final];
            int w = e.weight;

            if (distances[u] != INF && distances[u] + w < distances[v_idx]) {
                distances[v_idx] = distances[u] + w;
                predecessors[v_idx] = u;
                changed = true;
            }
        }
        if (!changed) break;
    }

    bool has_negative_cycle = false;
    for (const Edge& e : edges) {
        int u = vertex_to_index[e.init];
        int v_idx = vertex_to_index[e.final];
        int w = e.weight;

        if (distances[u] != INF && distances[u] + w < distances[v_idx]) {
            has_negative_cycle = true;
            break;
        }
    }

    if (has_negative_cycle) {
        throw runtime_error("Negative-weight cycle detected. No solution exists.");
    }

    // Вывод результатов (при необходимости)
    /*
    cout << "\nResults from " << start << ":\n";
    cout << "Vertex\tDistance\tPredecessor\n";
    for (const string& vert : vertices) {
        int idx = vertex_to_index[vert];
        cout << vert << "\t"
             << (distances[idx] == INT_MAX ? "INF" : to_string(distances[idx])) << "\t\t"
             << (predecessors[idx] == -1 ? "-" : vertices[predecessors[idx]]) << endl;
    }
    */
}


void Graph::printGraph() {
        printf("Vertices:\n");
        for (const auto& v : vertices) {
            printf("%s ", v.c_str());
        }
        printf("\n\nEdges:\n");
        
        for (const Edge& e : edges) {
            printf("%s -> %s (weight: %d)\n", 
                   e.init.c_str(), 
                   e.final.c_str(), 
                   e.weight);
        }
    }


Graph generate_random_graph(int num_vertices, int max_edges_per_vertex, 
                          int min_weight, int max_weight) {
    if (num_vertices <= 0) {
        throw invalid_argument("Number of vertices must be positive.");
    }
    if (max_edges_per_vertex < 0) {
        throw invalid_argument("Max edges per vertex cannot be negative.");
    }
    
    vector<string> vertices;
    for (int i = 0; i < num_vertices; ++i) {
        vertices.push_back("V" + to_string(i));
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> weight_dist(min_weight, max_weight);
    
    int actual_max_edges = min(max_edges_per_vertex, num_vertices - 1);
    uniform_int_distribution<> edge_count_dist(0, actual_max_edges);
    
    set<pair<string, string>> unique_edges;
    vector<Edge> edges;

    for (const string& u : vertices) {
        int edge_count = edge_count_dist(gen);
        
        vector<string> possible_targets;
        for (const string& v : vertices) {
            if (v != u) possible_targets.push_back(v);
        }
        
        shuffle(possible_targets.begin(), possible_targets.end(), gen);
        
        int created_edges = 0;
        for (const string& v : possible_targets) {
            if (created_edges >= edge_count) break;
            
            if (unique_edges.insert({u, v}).second) {
                Edge e;
                e.init = u;
                e.final = v;
                e.weight = weight_dist(gen);
                edges.push_back(e);
                created_edges++;
            }
        }
    }

    return Graph(vertices, edges);
}


int main() {
    try {
        
        Graph g = generate_random_graph(10, 5, -10, 10);
        
        auto start = chrono::high_resolution_clock::now();
        g.BellmanFord("V0");
        auto end = chrono::high_resolution_clock::now();
        
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        cout << "\nExecution time: " << duration.count() << " microseconds" << endl;

    } catch (const invalid_argument& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    } catch (const runtime_error& e) {
        cerr << "Error: " << e.what() << endl;
        return 2;
    }

    return 0;
}
