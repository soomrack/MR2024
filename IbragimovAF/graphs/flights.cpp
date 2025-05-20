#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>

class Edge;

class Node
{
public:
   std::string city_name;
   std::vector<Edge> edges;
   std::map<std::string, Edge> parents;

   // Конструктор по умолчанию
   Node() : city_name("") {}

   // Конструктор с параметром
   Node(std::string city_name) : city_name(city_name) {}
};

class Edge
{
public:
   std::string direction_city;
   int flight_time;

   // Конструктор по умолчанию
   Edge() : direction_city(""), flight_time(0) {}

   // Конструктор с параметрами
   Edge(std::string direction_city, int flight_time) : direction_city(direction_city), flight_time(flight_time) {}
};


Node &add_or_get_node(std::unordered_map<std::string, Node> &graph, const std::string &city_name)
{
   // Если города нет в графе, создаем новый узел
   if (graph.find(city_name) == graph.end())
   {
      graph[city_name] = Node(city_name);
   }
   return graph[city_name];
}

void add_edge(std::unordered_map<std::string, Node> &graph, const std::string &from_city, const std::string &to_city, int flight_time)
{
   // Получаем ссылки на узлы
   Node &from_node = add_or_get_node(graph, from_city);
   Node &to_node = add_or_get_node(graph, to_city);

   // Добавляем ребро в список edges
   from_node.edges.emplace_back(to_city, flight_time);

   // Добавляем обратную ссылку в parents
   to_node.parents[from_city] = Edge(from_city, flight_time);
}

void generate_graph(std::unordered_map<std::string, Node> &graph)
{
   graph.clear();

   std::ifstream input_file("filtered_data.csv");
   if (!input_file.is_open())
   {
      std::cerr << "Error opening file" << std::endl;
      return;
   }

   std::string header_line;
   getline(input_file, header_line); // Пропуск заголовка

   std::string data_line;
   while (getline(input_file, data_line))
   {
      std::stringstream ss(data_line);
      std::string from_city, to_city, flight_time_str;

      getline(ss, from_city, ',');
      getline(ss, to_city, ',');
      getline(ss, flight_time_str);

      try
      {
         int flight_time = std::stoi(flight_time_str);
         add_edge(graph, from_city, to_city, flight_time);
      }
      catch (const std::exception &e)
      {
         std::cerr << "Error parsing line: " << data_line << " - " << e.what() << std::endl;
      }
   }
}

void print_graph(const std::unordered_map<std::string, Node> &graph)
{
   for (const auto &pair : graph)
   {
      const Node &node = pair.second;
      std::cout << "City: " << node.city_name << "\nEdges:\n";
      for (const Edge &edge : node.edges)
      {
         std::cout << "  -> " << edge.direction_city << " (Flight time: " << edge.flight_time << ")\n";
      }
      std::cout << "Parents:\n";
      for (const auto &parent_pair : node.parents)
      {
         std::cout << "  <- " << parent_pair.first << " (Flight time: " << parent_pair.second.flight_time << ")\n";
      }
      std::cout << std::endl;
   }
}

std::vector<std::string> dijkstra(const std::unordered_map<std::string, Node> &graph, const std::string &start_city, const std::string &end_city, int &total_time)
{
   std::unordered_map<std::string, int> distances;
   std::unordered_map<std::string, std::string> previous;
   std::priority_queue<std::pair<int, std::string>,std::vector<std::pair<int, std::string>>,std::greater<>> min_heap;

   total_time = -1; // По умолчанию путь не найден

   for (const auto &pair : graph)
   {
      distances[pair.first] = std::numeric_limits<int>::max();
   }

   distances[start_city] = 0;
   min_heap.emplace(0, start_city);

   while (!min_heap.empty())
   {
      auto [current_dist, current_city] = min_heap.top();
      min_heap.pop();

      if (current_city == end_city)
         break;

      const auto &current_node = graph.at(current_city);
      for (const auto &edge : current_node.edges)
      {
         const std::string &neighbor = edge.direction_city;
         int new_dist = current_dist + edge.flight_time;

         if (new_dist < distances[neighbor])
         {
            distances[neighbor] = new_dist;
            previous[neighbor] = current_city;
            min_heap.emplace(new_dist, neighbor);
         }
      }
   }

   std::vector<std::string> path;
   if (distances[end_city] == std::numeric_limits<int>::max())
   {
      std::cout << "No path found from " << start_city << " to " << end_city << std::endl;
      return path;
   }

   // Восстановление пути
   for (std::string at = end_city; !at.empty(); at = previous.count(at) ? previous[at] : "")
   {
      path.push_back(at);
   }
   std::reverse(path.begin(), path.end());

   total_time = distances[end_city]; // Общее время пути

   return path;
}

void print_path(const std::vector<std::string> &path, int total_time)
{
   std::cout << "Shortest path: ";
   for (size_t i = 0; i < path.size(); ++i)
   {
      std::cout << path[i];
      if (i != path.size() - 1) std::cout << " -> ";
   }
   std::cout << "\nTotal flight time: " << total_time << " minutes" << std::endl;
}

int main()
{
   std::unordered_map<std::string, Node> graph;
   generate_graph(graph);
   print_graph(graph);

   std::string from = "ANC";
   std::string to = "KWT";

   int total_time = 0;
   std::vector<std::string> path = dijkstra(graph, from, to, total_time);

   print_path(path, total_time);
}