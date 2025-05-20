#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

using namespace std;

class Graph {
    int V; // Количество вершин
    vector<vector<int>> adj; // Список смежности
    
    // Рекурсивная функция для DFS
    void DFSUtil(int v, vector<bool>& visited, vector<int>& component) {
        visited[v] = true;
        component.push_back(v);
        
        for (int u : adj[v]) {
            if (!visited[u]) {
                DFSUtil(u, visited, component);
            }
        }
    }
    
public:
    Graph(int V) : V(V), adj(V) {}
    
    // Добавление ребра в граф
    void addEdge(int v, int w) {
        adj[v].push_back(w);
    }
    
    // Получение транспонированного графа
    Graph getTranspose() {
        Graph g(V);
        for (int v = 0; v < V; v++) {
            for (int u : adj[v]) {
                g.addEdge(u, v);
            }
        }
        return g;
    }
    
    // Заполнение стека вершинами в порядке завершения обработки
    void fillOrder(int v, vector<bool>& visited, stack<int>& Stack) {
        visited[v] = true;
        
        for (int u : adj[v]) {
            if (!visited[u]) {
                fillOrder(u, visited, Stack);
            }
        }
        
        // Все достижимые из v вершины обработаны, добавляем v в стек
        Stack.push(v);
    }
    
    // Основная функция для нахождения SCC
    vector<vector<int>> getSCCs() {
        stack<int> Stack;
        vector<bool> visited(V, false);
        
        // Заполняем стек в порядке завершения обработки вершин
        for (int i = 0; i < V; i++) {
            if (!visited[i]) {
                fillOrder(i, visited, Stack);
            }
        }
        
        // Создаем транспонированный граф
        Graph gr = getTranspose();
        
        // Снова помечаем все вершины как не посещенные
        fill(visited.begin(), visited.end(), false);
        
        vector<vector<int>> sccs;
        
        // Обрабатываем вершины в порядке, определенном стеком
        while (!Stack.empty()) {
            int v = Stack.top();
            Stack.pop();
            
            if (!visited[v]) {
                vector<int> component;
                gr.DFSUtil(v, visited, component);
                sccs.push_back(component);
            }
        }
        
        return sccs;
    }
};

int main() {
    // Создаем граф
    Graph g(5);
    g.addEdge(1, 0);
    g.addEdge(0, 2);
    g.addEdge(2, 1);
    g.addEdge(0, 3);
    g.addEdge(3, 4);
    
    // Находим сильно связанные компоненты
    vector<vector<int>> sccs = g.getSCCs();
    
    // Выводим результат
    cout << "Strongly Connected Components:\n";
    for (const auto& scc : sccs) {
        for (int v : scc) {
            cout << v << " ";
        }
        cout << endl;
    }
    
    return 0;
}
