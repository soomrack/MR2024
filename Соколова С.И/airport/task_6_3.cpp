#pragma execution_character_set("utf-8")


#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <limits>
#include <string>
#include <algorithm>
#include <cctype>


#define NOMINMAX


#include <windows.h>


class Graph {
private:
	std::vector<std::vector<float>> airTimeMatrix;
	std::unordered_map<std::string, int> airportToIndex;
	std::vector<std::string> indexToAirport;

public:
	void loadFromCSV(const std::string& fileName);
	float getEdgeWeight(const std::string& from, const std::string& to);
	int getIndex(const std::string& airportName);
	std::string getAirport(int index);
	int nodesNumber() const;
	const std::vector<std::vector<float>>& getMatrix() const;
	std::pair<std::vector<std::string>, float> findShortestPath(
		const std::string& from, 
		const std::string& to
	) const;
};


//удаление пробелов/символов переноса в начале и в конце — после чтения названий аэропортов.
std::string trim(const std::string& str) 
{
	size_t first = str.find_first_not_of(" \t\n\r");
	if (first == std::string::npos) return "";
	size_t last = str.find_last_not_of(" \t\n\r");
	return str.substr(first, (last - first + 1));
}


// Функция чтения из файла
void Graph::loadFromCSV(const std::string& fileName) 
{
	std::ifstream file(fileName);
	if (!file.is_open()) {
		std::cerr << "Ошибка чтения файла\n";
		return;
	}

	std::string line;
	std::getline(file, line); // Пропустить заголовок файла

	int currentIndex = 0;
	std::vector<std::tuple<std::string, std::string, float>> edges; // Массив из кортежей, которые хранят имя аэропорта вылета и назначения и время полета

	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string air_time_str, origin, dest;

		if (!std::getline(ss, air_time_str, ';') ||
			!std::getline(ss, origin, ';') ||
			!std::getline(ss, dest, ';')) {
			std::cerr << "Строка пропущена: недостаточно полей: " << line << '\n';
			continue;
		}

		air_time_str = trim(air_time_str);
		origin = trim(origin);
		dest = trim(dest);

		// Если хотя бы одно из полей пустое, пропускаем эту строку, идем к следующей
		if (air_time_str.empty() || origin.empty() || dest.empty()) {
			std::cerr << "Строка пропущена из-за пустых значений: " << line << '\n';
			continue;
		}

		try {
			float air_time = std::stof(air_time_str);

			if (airportToIndex.find(origin) == airportToIndex.end()) {
				airportToIndex[origin] = currentIndex++; // если такого названия еще нет, то мы присваиваем ему текущий индекс, а затем увеличиваем его на 1
				indexToAirport.push_back(origin); // Добавляем элемент в конец вектора
			}
			if (airportToIndex.find(dest) == airportToIndex.end()) {
				airportToIndex[dest] = currentIndex++; // если такого названия еще нет, то мы присваиваем ему текущий индекс, а затем увеличиваем его на 1
				indexToAirport.push_back(dest);//  Добавляем элемент в конец вектора
			}

			edges.emplace_back(origin, dest, air_time);
		}
		catch (const std::exception&) {
			std::cerr << "Ошибка преобразования времени полета: '" << air_time_str << "' в строке: " << line << '\n';
			continue;
		}
	}
	file.close();

	int size = currentIndex;
	// Создает одно строку матрицы из size элементов и заполняет ихзначением бесконечность
	airTimeMatrix.assign(size, std::vector<float>(size, std::numeric_limits<float>::max()));

	// Заполняем матрицу на основе ребер
	for (const auto& [origin, dest, air_time] : edges) {
		int i = airportToIndex[origin];
		int j = airportToIndex[dest];
		airTimeMatrix[i][j] = std::min(airTimeMatrix[i][j], air_time);
	}
}


// Получить время полета между двумя пунктами
float Graph::getEdgeWeight(const std::string& from, const std::string& to) 
{
	int i = getIndex(from);
	int j = getIndex(to);
	return airTimeMatrix[i][j];
}


// Получить индекс по названию аэропорта
int Graph::getIndex(const std::string& airportName) // для себя способ передать строку без копировани в память 
{
	auto it = airportToIndex.find(airportName);
	if (it != airportToIndex.end()) return it->second;
	throw std::runtime_error("Аэропорт не найден: " + airportName);
}


// Получить название аэропорта по индексу
std::string Graph::getAirport(int index) 
{
	return indexToAirport[index];
}


// Получить количество узлов в графе
int Graph::nodesNumber() const 
{
	return airTimeMatrix.size();
}


const std::vector<std::vector<float>>& Graph::getMatrix() const 
{
	return airTimeMatrix;
}


// Метод Дейкстры
std::pair<std::vector<std::string>, float> Graph::findShortestPath(
	const std::string& from, 
	const std::string& to) 
	const 
{
	if (airportToIndex.find(from) == airportToIndex.end() || airportToIndex.find(to) == airportToIndex.end()) {
		throw std::runtime_error("Один или оба аэропорта не найдены.");
	}

	if (from == to) {
		throw std::runtime_error("Аэропорты вылета и прибытия совпадают.");
	}

	int nodes = airTimeMatrix.size(); // число узлов в графе
	int start = airportToIndex.at(from); // индекс начального аэропорта
	int end = airportToIndex.at(to); // индекс конечного аэропорта

	std::vector<float> dist(nodes, std::numeric_limits<float>::max()); // Расстояние до узлов, Изначально считаем, что аэпоропорт назначения находится бесконечно далеко
	std::vector<int> prev(nodes, -1); // хранит предыдущую вершину на кратчайшем пути к каждой вершине
	std::vector<bool> visited(nodes, false); //  отмечает, какие вершины уже обработаны

	dist[start] = 0.0f; // расстояние до самого себя

	for (int idx = 0; idx < nodes; ++idx) {
		// Найти вершину с минимальным расстоянием
		int u = -1;
		float minDist = std::numeric_limits<float>::max();

		for (int jdx = 0; jdx < nodes; ++jdx) {
			// Если мы не посетили еще вершину и расстояние меньше минимального
			if (!visited[jdx] && dist[jdx] < minDist) {
				u = jdx;  // сохраняем вершину
				minDist = dist[jdx]; // обновляем минимальное
			}
		}

		if (u == -1) break; // Остальные недостижимы

		visited[u] = true; // Отмечаем посещенную вершину

		// Рассматриваем соседей u
		for (int v = 0; v < nodes; ++v) {
			// Вершина еще не обработала и между ними существует ребро
			if (!visited[v] && airTimeMatrix[u][v] < std::numeric_limits<float>::max()) {
				float altDist = dist[u] + airTimeMatrix[u][v]; // альтернативное расстояние от u до v
				if (altDist < dist[v]) {
					dist[v] = altDist;
					prev[v] = u; // откуда мы пришли к v
				}
			}
		}
	}

	if (dist[end] == std::numeric_limits<float>::max()) {
		throw std::runtime_error("Путь не найден.");
	}

	// Построение пути
	std::vector<std::string> path;
	for (int at = end; at != -1; at = prev[at]) { // Идем от конца к началу
		path.push_back(indexToAirport[at]);
	}
	std::reverse(path.begin(), path.end());

	return { path, dist[end] };
}

int main() {
	// Устанавливаем кодировку консоли на UTF-8
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	try {
		Graph airGraph;
		airGraph.loadFromCSV("cleaned_flights.csv");

		if (airGraph.nodesNumber() == 0) {
			std::cerr << "Граф не загружен. Проверьте файл.\n";
			return 1;
		}

		std::string from, to;
		std::cout << "Введите код аэропорта вылета (например, JFK):\n";
		std::cin >> from;
		std::cout << "\nВведите код аэропорта прибытия (например, LAX):\n";
		std::cin >> to;

		from = trim(from);
		to = trim(to);

		auto [path, distance] = airGraph.findShortestPath(from, to);

		std::cout << "\nКратчайший путь по времени полета:\n";
		for (size_t i = 0; i < path.size(); ++i) {
			std::cout << path[i];
			if (i + 1 < path.size()) std::cout << " → ";
		}

		std::cout << "\nОбщее время полета: " << distance << " минут\n";
	}
	catch (const std::exception& ex) {
		std::cerr << "Ошибка: " << ex.what() << '\n';
	}
	return 0;
}
