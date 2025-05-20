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


void Graph::FloydWarshall() {
    int N = vertices.size();
    vector<vector<int>> distance(N, vector<int>(N, INT_MAX));
    vector<vector<int>> predecessors(N, vector<int>(N, -1));

    for (int i = 0; i < N; ++i) {
        distance[i][i] = 0;
        predecessors[i][i] = i;
    }

    for (const Edge& e : edges) {
        int u = vertex_to_index[e.init];
        int v = vertex_to_index[e.final];
        distance[u][v] = e.weight;
        predecessors[u][v] = u;
    }

    for (int k = 0; k < N; ++k) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                if (distance[i][k] != INT_MAX && distance[k][j] != INT_MAX) {
                    if (distance[i][j] > distance[i][k] + distance[k][j]) {
                        distance[i][j] = distance[i][k] + distance[k][j];
                        predecessors[i][j] = predecessors[k][j];
                    }
                }
            }
        }
    }

    for (int i = 0; i < N; ++i) {
        if (distance[i][i] < 0) {
            throw runtime_error("Graph contains a negative weight cycle.");
        }
    }

    cout << "Shortest distances matrix:\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (distance[i][j] == INT_MAX)
                cout << "INF\t";
            else
                cout << distance[i][j] << "\t";
        }
        cout << "\n";
    }

    cout << "\nPredecessors matrix:\n";
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (predecessors[i][j] == -1)
                cout << "NIL\t";
            else
                cout << vertices[predecessors[i][j]] << "\t";
        }
        cout << "\n";
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
        g.FloydWarshall();
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
