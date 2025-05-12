#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>


class Vertex {
public:
    int prev_vert = 0;
    int time_from_start = std::numeric_limits<int>::max();
};


class Neighbor {
public:
    int vertex;
    int time;
};


struct MinHeap {
private:
    std::vector<Neighbor> heap;
public:
    Neighbor& operator[](size_t index)
    {
        return heap[index];
    }

    size_t size() const
    {
        return heap.size();
    }

    bool empty() const
    {
        return heap.empty();
    }

    void heapify_down(int idx) 
    {
        int left = 2 * idx + 1;
        int right = 2 * idx + 2;
        int smallest = idx;

        if (left < heap.size() && heap[left].time < heap[smallest].time)
        {
            smallest = left;
        }
        if (right < heap.size() && heap[right].time < heap[smallest].time)
        {
            smallest = right;
        }

        if (smallest != idx)
        {
            std::swap(heap[idx], heap[smallest]);
            heapify_down(smallest);
        }
    }

    void heapify_up(size_t idx)
    {
        size_t parent = (idx - 1) / 2;
        while (idx > 0 && heap[idx].time < heap[parent].time)
        {
            std::swap(heap[idx], heap[parent]);
            idx = parent;
            parent = (idx - 1) / 2;
        }
    }

    void push(const Neighbor& neighbor) 
    {
        heap.push_back(neighbor);
        heapify_up(heap.size() - 1);
    }

    Neighbor pop()
    {
        if (heap.empty())
        {
            throw std::out_of_range("Heap is empty");
        }

        Neighbor min = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapify_down(0);

        return min;
    }
};


void dijkstra(int start, int end, const std::vector<std::vector<int>>& fl_table)
{
    size_t table_size = fl_table.size();
    std::vector<Vertex> Verts(table_size);
    std::vector<int> stack_to_go;
    std::vector<bool> in_heap(table_size, false);
    MinHeap heap;

    heap.push({ start, 0 });
    Verts[start].time_from_start = 0;

    while (!heap.empty())
    {
        int current_vertex = heap.pop().vertex;

        for (size_t neighbor = 0; neighbor < table_size; ++neighbor)
        {
            if (fl_table[current_vertex][neighbor] != std::numeric_limits<int>::max())
            {
                stack_to_go.push_back(neighbor);
            }
        }

        for (int neighbor : stack_to_go)
        {
            int new_time = Verts[current_vertex].time_from_start + fl_table[current_vertex][neighbor];
            if (new_time < Verts[neighbor].time_from_start)
            {
                Verts[neighbor].time_from_start = new_time;
                Verts[neighbor].prev_vert = current_vertex;
                if (!in_heap[neighbor])
                {
                    heap.push({ neighbor, new_time });
                    in_heap[neighbor] = true;
                }
                else
                {
                    for (size_t idx = 0; idx < heap.size(); ++idx)
                    {
                        if (heap[idx].vertex == neighbor)
                        {
                            heap[idx].time = new_time;
                            heap.heapify_up(idx);
                            heap.heapify_down(idx);
                        }
                    }
                }
            }
        }
        stack_to_go.clear();
    }

    if (Verts[end].time_from_start == std::numeric_limits<int>::max()) // вывод результата 
    {
        std::cout << "No path found" << std::endl;
    }
    else
    {
        std::cout << "Shortest path: ";

        for (int idx_vert = end; idx_vert != start; idx_vert = Verts[idx_vert].prev_vert)
        {
            std::cout << idx_vert << " <- ";
        }
        std::cout << start << std::endl;
        std::cout << "Time: " << Verts[end].time_from_start << std::endl;
    }
}


void filling_table(std::vector<std::vector<int>>& flight_table)
{
    std::ifstream datafile;
    datafile.open("logfile.txt");

    if (!datafile.is_open())
    {
        std::cout << "Couldn't open datafile" << std::endl;
        return;
    }

    std::string txtline;
    while (!datafile.eof())
    {
        getline(datafile, txtline);
        std::stringstream inputString(txtline);

        int str, col, time;
        std::string tempString;

        getline(inputString, tempString, '\t');
        str = atoi(tempString.c_str());

        getline(inputString, tempString, '\t');
        col = atoi(tempString.c_str());

        getline(inputString, tempString, '\t');
        time = atoi(tempString.c_str());

        if (flight_table[str][col] > time)
        {
            flight_table[str][col] = time;
        }
    }
}


int main() {
    int table_size = 40 * 1000;
    int start, end;
    unsigned int state, run = 1;

    std::vector<std::vector<int>> flight_table(table_size, std::vector<int>(table_size, std::numeric_limits<int>::max()));

    filling_table(flight_table);

    while (run)
    {
        std::cout << "To start press 1: ";
        std::cin >> state;

        switch (state)
        {
        case 0:
            run = 0;
            break;
        case 1:
            std::cout << "Start: ";
            std::cin >> start;
            std::cout << "End: ";
            std::cin >> end;
            dijkstra(start, end, flight_table);
            break;
        }
    }

    return 0;
}