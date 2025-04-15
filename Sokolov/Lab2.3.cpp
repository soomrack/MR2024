#include <atomic>
#include <chrono>
#include <condition_variable>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// constants
const int min_connection_time = 3600; // 1 hour in seconds
const int min_frequency = 1;          // minimum flight frequency

// ===================================================
// enum for flight_category – to avoid magic strings
// ===================================================
enum class flight_category { domestic, international, unknown };

std::string flight_category_to_string(flight_category fc) {
    switch (fc) {
    case flight_category::domestic: return "Domestic";
    case flight_category::international: return "International";
    default: return "Unknown";
    }
}

flight_category parse_flight_category(const std::string& s) {
    std::string lower = s;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    if (lower == "d" || lower == "du")
        return flight_category::domestic;
    if (lower == "i")
        return flight_category::international;
    return flight_category::unknown;
}

// utility function to parse a date/time string in "YYYY-MM-DD HH:MM" format
bool parse_date_time(const std::string& date_time_str, time_t& time_out) {
    std::istringstream ss(date_time_str);
    std::tm tm_val = {};
    ss >> std::get_time(&tm_val, "%Y-%m-%d %H:%M");
    if (ss.fail())
        return false;
    time_out = mktime(&tm_val);
    return true;
}

// ==============================
// class flight – represents a flight
// ==============================
class flight {
public:
    // default constructor
    flight()
        : origin(""), destination(""), frequency(0), category(flight_category::unknown),
        duration(0), distance(0.0) {
    }

    flight(const std::string& orig, const std::string& dest, int freq,
        flight_category cat, time_t dur, double dist)
        : origin(orig), destination(dest), frequency(freq), category(cat),
        duration(dur), distance(dist) {
    }

    const std::string& get_origin() const { return origin; }
    const std::string& get_destination() const { return destination; }
    int get_frequency() const { return frequency; }
    flight_category get_category() const { return category; }
    time_t get_duration() const { return duration; }
    double get_distance() const { return distance; }

    void print() const {
        int dur_minutes = static_cast<int>(duration / 60);
        std::cout << origin << " -> " << destination
            << " | Duration: " << dur_minutes << " minutes"
            << " | Frequency: " << frequency
            << " | Category: " << flight_category_to_string(category)
            << " | Distance: " << distance << " miles" << std::endl;
    }

private:
    std::string origin;       // ORIGIN (airport code)
    std::string destination;  // DEST (airport code)
    int frequency;            // DEPARTURES_PERFORMED
    flight_category category; // parsed from DATA_SOURCE field
    time_t duration;          // flight duration in seconds (from AIR_TIME in minutes)
    double distance;          // DISTANCE in miles
};

// =====================================================
// LRU cache for route caching with dynamic resizing
// =====================================================
template <typename Key, typename Value>
class dynamic_lru_cache {
public:
    dynamic_lru_cache(size_t initial_capacity) : capacity_(initial_capacity), min_capacity_(10), max_capacity_(1000) {}

    bool get(const Key& key, Value& value) {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        auto it = cache_items_map_.find(key);
        if (it == cache_items_map_.end())
            return false;
        // Move item to front
        cache_items_list_.splice(cache_items_list_.begin(), cache_items_list_, it->second);
        value = it->second->second;
        return true;
    }

    void put(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(cache_mutex_);
        auto it = cache_items_map_.find(key);
        if (it != cache_items_map_.end()) {
            // Update and move to front
            it->second->second = value;
            cache_items_list_.splice(cache_items_list_.begin(), cache_items_list_, it->second);
            return;
        }

        // Check if we need to resize the cache
        adjust_capacity();

        if (cache_items_list_.size() >= capacity_) {
            // Remove least recently used
            auto last = cache_items_list_.end();
            last--;
            cache_items_map_.erase(last->first);
            cache_items_list_.pop_back();
        }
        cache_items_list_.push_front(std::make_pair(key, value));
        cache_items_map_[key] = cache_items_list_.begin();
    }

private:
    void adjust_capacity() {
        // Example adjustment logic: increase capacity if usage is high, decrease if usage is low
        size_t current_size = cache_items_list_.size();
        if (current_size > capacity_ * 0.75 && capacity_ < max_capacity_) {
            capacity_ *= 2;
            if (capacity_ > max_capacity_) {
                capacity_ = max_capacity_;
            }
        }
        else if (current_size < capacity_ * 0.25 && capacity_ > min_capacity_) {
            capacity_ /= 2;
            if (capacity_ < min_capacity_) {
                capacity_ = min_capacity_;
            }
        }
    }

    std::list<std::pair<Key, Value>> cache_items_list_;
    std::unordered_map<Key, typename std::list<std::pair<Key, Value>>::iterator> cache_items_map_;
    size_t capacity_;
    const size_t min_capacity_;
    const size_t max_capacity_;
    std::mutex cache_mutex_;
};

// =====================================================
// class file_reader – reads and parses the CSV file using a thread pool (without std::async)
// =====================================================
class file_reader {
public:
    static std::vector<flight> read_flights_from_file(const std::string& filename) {
        std::vector<flight> flights;
        std::ifstream infile(filename);
        if (!infile) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return flights;
        }
        std::string header_line;
        if (!std::getline(infile, header_line)) {
            std::cerr << "Error reading header line." << std::endl;
            return flights;
        }
        auto header_map = parse_header(header_line);

        // Read all remaining lines into a vector
        std::vector<std::string> all_lines;
        std::string line;
        while (std::getline(infile, line)) {
            if (!line.empty())
                all_lines.push_back(line);
        }

        // Determine dynamic pool size: not more than available cores, but also not more than needed.
        unsigned available_threads = std::thread::hardware_concurrency();
        unsigned pool_size = std::min(available_threads, static_cast<unsigned>(all_lines.size()));
        if (pool_size == 0) pool_size = 1;

        std::vector<flight> result;
        result.reserve(all_lines.size());
        std::mutex result_mutex;

        // Shared index for dynamic work distribution
        std::atomic<size_t> line_index(0);

        auto worker = [&]() {
            while (true) {
                size_t index = line_index.fetch_add(1);
                if (index >= all_lines.size())
                    break;
                const std::string& curr_line = all_lines[index];
                try {
                    // Preliminary filter: if line does not contain 'D' or 'd', skip it.
                    if (curr_line.find("D") == std::string::npos &&
                        curr_line.find("d") == std::string::npos)
                        continue;
                    flight fl = parse_line(curr_line, header_map);
                    if (!fl.get_origin().empty()) {
                        std::lock_guard<std::mutex> lock(result_mutex);
                        result.push_back(std::move(fl));
                    }
                }
                catch (const std::exception& e) {
                    std::lock_guard<std::mutex> lock(result_mutex);
                    std::cerr << "Error parsing line: " << curr_line << "\n" << e.what() << std::endl;
                }
            }
            };

        std::vector<std::thread> threads;
        for (unsigned i = 0; i < pool_size; ++i)
            threads.emplace_back(worker);
        for (auto& th : threads)
            th.join();

        return result;
    }
private:
    static std::unordered_map<std::string, int> parse_header(const std::string& header_line) {
        std::unordered_map<std::string, int> header_map;
        std::istringstream ss(header_line);
        std::string token;
        int index = 0;
        while (std::getline(ss, token, ',')) {
            header_map[token] = index++;
        }
        return header_map;
    }

    // Parses a CSV data line into a flight object.
    // Expected fields: ORIGIN, DEST, DEPARTURES_PERFORMED, AIR_TIME, DISTANCE, DATA_SOURCE.
    static flight parse_line(const std::string& line,
        const std::unordered_map<std::string, int>& header_map) {
        std::istringstream ss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        if (tokens.size() < 50)
            throw std::runtime_error("Not enough data in line.");
        // Additional checks for required fields:
        if (tokens.at(header_map.at("ORIGIN")).empty() ||
            tokens.at(header_map.at("DEST")).empty() ||
            tokens.at(header_map.at("AIR_TIME")).empty())
            throw std::runtime_error("Missing required fields.");

        std::string origin = tokens.at(header_map.at("ORIGIN"));
        std::string dest = tokens.at(header_map.at("DEST"));
        int freq = std::stoi(tokens.at(header_map.at("DEPARTURES_PERFORMED")));
        double air_time_minutes = std::stod(tokens.at(header_map.at("AIR_TIME")));
        time_t duration = static_cast<time_t>(air_time_minutes * 60);
        double distance = std::stod(tokens.at(header_map.at("DISTANCE")));
        std::string data_source = tokens.at(header_map.at("DATA_SOURCE"));
        flight_category cat = parse_flight_category(data_source);

        return flight(origin, dest, freq, cat, duration, distance);
    }
};

// =====================================================
// class flight_filter – filters flights based on criteria
// =====================================================
class flight_filter {
public:
    // Returns only flights with frequency >= min_frequency and category domestic.
    static std::vector<flight> filter_flights(const std::vector<flight>& all_flights) {
        std::vector<flight> valid_flights;
        valid_flights.reserve(all_flights.size() / 2);
        for (const auto& fl : all_flights) {
            if (is_valid(fl))
                valid_flights.push_back(fl);
        }
        return valid_flights;
    }
private:
    static bool is_valid(const flight& fl) {
        if (fl.get_frequency() < min_frequency)
            return false;
        return (fl.get_category() == flight_category::domestic);
    }
};

// =====================================================
// class flight_graph – represents the flight graph using std::map
// =====================================================
class flight_graph {
public:
    void add_flight(const flight& fl) {
        flights_map[fl.get_origin()].push_back(fl);
    }
    const std::vector<flight>& get_flights_from(const std::string& airport) const {
        static const std::vector<flight> empty;
        auto it = flights_map.find(airport);
        return (it != flights_map.end()) ? it->second : empty;
    }
    void sort_flights() {
        for (auto& pair : flights_map) {
            std::sort(pair.second.begin(), pair.second.end(),
                [](const flight& a, const flight& b) {
                    return a.get_duration() < b.get_duration();
                });
        }
    }
    // Simple BFS reachability check
    bool is_reachable(const std::string& start, const std::string& destination) const {
        std::queue<std::string> q;
        std::unordered_map<std::string, bool> visited;
        q.push(start);
        visited[start] = true;
        while (!q.empty()) {
            std::string cur = q.front();
            q.pop();
            if (cur == destination)
                return true;
            for (const auto& fl : get_flights_from(cur)) {
                std::string next = fl.get_destination();
                if (!visited[next]) {
                    visited[next] = true;
                    q.push(next);
                }
            }
        }
        return false;
    }
private:
    std::map<std::string, std::vector<flight>> flights_map;
};

// =====================================================
// class shortest_path_finder – finds the fastest route using A* (with zero heuristic) and caching
// =====================================================
class shortest_path_finder {
public:
    shortest_path_finder(const flight_graph& graph) : flight_graph_(graph) {}

    std::vector<flight> find_shortest_path(const std::string& start,
        const std::string& destination,
        time_t user_departure_time) {
        std::string key = start + "_" + destination + "_" + std::to_string(user_departure_time);
        std::vector<flight> cached_route;
        if (route_cache_.get(key, cached_route))
            return cached_route;

        if (!flight_graph_.is_reachable(start, destination)) {
            std::cerr << "Destination not reachable from start." << std::endl;
            return {};
        }
        std::unordered_map<std::string, time_t> best_time;
        std::unordered_map<std::string, flight> prev_flight;
        using node = std::pair<time_t, std::string>;
        auto cmp = [](const node& a, const node& b) { return a.first > b.first; };
        std::priority_queue<node, std::vector<node>, decltype(cmp)> pq(cmp);

        best_time[start] = user_departure_time;
        pq.push({ user_departure_time, start });

        while (!pq.empty()) {
            node top = pq.top();
            pq.pop();
            time_t current_time = top.first;
            std::string airport = top.second;
            if (airport == destination)
                break;
            if (current_time > best_time[airport])
                continue;
            const auto& fls = flight_graph_.get_flights_from(airport);
            for (const auto& fl : fls) {
                time_t depart_time = (airport == start) ? current_time : current_time + min_connection_time;
                // A* heuristic: using zero (can be replaced with a proper heuristic)
                time_t heuristic = 0;
                time_t estimated_arrival = depart_time + fl.get_duration() + heuristic;
                if (best_time.find(fl.get_destination()) == best_time.end() ||
                    estimated_arrival < best_time[fl.get_destination()]) {
                    best_time[fl.get_destination()] = estimated_arrival;
                    prev_flight[fl.get_destination()] = fl;
                    pq.push({ estimated_arrival, fl.get_destination() });
                }
            }
        }
        if (best_time.find(destination) == best_time.end())
            return {};
        std::vector<flight> path;
        std::string curr = destination;
        while (curr != start) {
            if (prev_flight.find(curr) == prev_flight.end())
                break;
            flight fl = prev_flight[curr];
            path.push_back(fl);
            curr = fl.get_origin();
        }
        std::reverse(path.begin(), path.end());
        route_cache_.put(key, path);
        return path;
    }

private:
    const flight_graph& flight_graph_;
    static dynamic_lru_cache<std::string, std::vector<flight>> route_cache_;
};

// Initialize static member
template <>
dynamic_lru_cache<std::string, std::vector<flight>> shortest_path_finder::route_cache_{ 100 };

// =====================================================
// Function get_user_input – obtains input from the user (in English)
// =====================================================
bool get_user_input(std::string& start_airport, std::string& destination_airport, time_t& user_departure_time) {
    std::cout << "Enter departure airport code: ";
    if (!std::getline(std::cin, start_airport) || start_airport.empty()) {
        std::cerr << "Invalid departure airport input." << std::endl;
        return false;
    }
    std::cout << "Enter destination airport code: ";
    if (!std::getline(std::cin, destination_airport) || destination_airport.empty()) {
        std::cerr << "Invalid destination airport input." << std::endl;
        return false;
    }
    std::cout << "Enter departure date and time (YYYY-MM-DD HH:MM): ";
    std::string date_time_str;
    if (!std::getline(std::cin, date_time_str) || date_time_str.empty()) {
        std::cerr << "Invalid date/time input." << std::endl;
        return false;
    }
    if (!parse_date_time(date_time_str, user_departure_time)) {
        std::cerr << "Error: Incorrect date/time format." << std::endl;
        return false;
    }
    return true;
}

// =====================================================
// Function print_shortest_path – prints the route and additional statistics
// =====================================================
void print_shortest_path(const std::vector<flight>& path, time_t user_departure_time) {
    if (path.empty()) {
        std::cout << "No route found." << std::endl;
        return;
    }
    std::cout << "Route found:" << std::endl;
    double total_distance = 0.0;
    time_t total_flight_duration = 0;
    for (const auto& fl : path) {
        fl.print();
        total_distance += fl.get_distance();
        total_flight_duration += fl.get_duration();
    }
    // Calculate total travel time (flight durations + connection delays)
    time_t current_time = user_departure_time;
    for (size_t i = 0; i < path.size(); i++) {
        if (i > 0)
            current_time += min_connection_time;
        current_time += path[i].get_duration();
    }
    time_t total_travel_time = current_time - user_departure_time;
    int hours = static_cast<int>(total_travel_time / 3600);
    int minutes = static_cast<int>((total_travel_time % 3600) / 60);
    double avg_speed = (total_flight_duration > 0) ? (total_distance / (total_flight_duration / 3600.0)) : 0.0;

    std::cout << "Total flight distance: " << total_distance << " miles" << std::endl;
    std::cout << "Total flight duration: " << total_flight_duration / 60 << " minutes" << std::endl;
    std::cout << "Average flight speed: " << avg_speed << " mph" << std::endl;
    std::cout << "Total travel time: " << hours << " hours " << minutes << " minutes." << std::endl;
}

// =====================================================
// Main function – a balanced overview of program steps
// =====================================================
int main() {
    std::string departure_airport, destination_airport;
    time_t departure_time;

    // Step 1: Get user input
    if (!get_user_input(departure_airport, destination_airport, departure_time)) {
        std::cerr << "User input error. Exiting." << std::endl;
        return 1;
    }

    // Step 2: Read flight data using a thread pool
    std::vector<flight> all_flights = file_reader::read_flights_from_file("Graph.csv");
    if (all_flights.empty()) {
        std::cerr << "No flight data available." << std::endl;
        return 1;
    }

    // Step 3: Filter flights based on criteria
    std::vector<flight> valid_flights = flight_filter::filter_flights(all_flights);
    if (valid_flights.empty()) {
        std::cerr << "No valid flights found after filtering." << std::endl;
        return 1;
    }

    // Step 4: Build flight graph and sort flights
    flight_graph fg;
    for (const auto& fl : valid_flights)
        fg.add_flight(fl);
    fg.sort_flights();

    // Step 5: Find fastest route using A* (with zero heuristic) and caching
    shortest_path_finder spf(fg);
    std::vector<flight> best_path = spf.find_shortest_path(departure_airport, destination_airport, departure_time);

    // Step 6: Print result and additional statistics
    print_shortest_path(best_path, departure_time);

    return 0;
}