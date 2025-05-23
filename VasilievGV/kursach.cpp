#include <vector>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <algorithm> // для reverse

#define COLOR_OF_NODE_GREY 1
#define COLOR_OF_NODE_BLACK 2

typedef std::vector<unsigned int> uiVector;

class node
{
private:
    int value;
    uiVector edges;
public:
    int color;

    node()
    {
        this->value = 0;
        this->color = 0;
    }

    node(int val)
    {
        this->value = val;
        this->color = 0;
    }

    void setValue(int val)
    {
        this->value = val;
    }

    unsigned int edge(unsigned int num)
    {
        return this->edges[num];
    }

    unsigned int edgeSize()
    {
        return this->edges.size();
    }

    int getValueOfNode()
    {
        return this->value;
    }

    void addEdge(unsigned int link)
    {
        edges.push_back(link);
    }

    ~node()
    {
        edges.clear();
    }
};

class graph
{
private:
    unsigned int amount;
    node * nodes;
    bool dfs(unsigned int curNode, uiVector &stack);
public:
    graph(unsigned int size);
    void print();
    bool topological_sort(uiVector &result);
    ~graph()
    {
        delete[] nodes;
    }
};

static unsigned int random_Range(unsigned int range)
{
    return rand() % range;
}

graph::graph(unsigned int size)
{
    std::srand(std::time(0));
    this->nodes = new node[size];
    this->amount = size;

    unsigned int ranks[size][size + 1];
    memset(ranks, 0, size * (size + 1) * sizeof(int));

    for (unsigned int i = 0; i < size; i++)
    {
        nodes[i].setValue(i);
        unsigned int currentRank = random_Range(size);
        ranks[currentRank][0] += 1;
        ranks[currentRank][ranks[currentRank][0]] = i;
    }

    for (unsigned int rank = 0; rank < size - 1; rank++)
    {
        for (unsigned int curNode = 1; curNode < ranks[rank][0] + 1; curNode++)
        {
            for (unsigned int rankLow = rank + 1; rankLow < size; rankLow++)
            {
                for (unsigned int link = 1; link < ranks[rankLow][0] + 1; link++)
                {
                    if (random_Range(100) > 50)
                    {
                        nodes[ranks[rank][curNode]].addEdge(ranks[rankLow][link]);
                    }
                }
            }
        }
    }
}

void graph::print()
{
    for (unsigned int i = 0; i < this->amount; i++)
    {
        int value = this->nodes[i].getValueOfNode();
        std::cout << "Node " << i << ":  " << value << ";    edges:";
        unsigned int edgeCount = this->nodes[i].edgeSize();
        for (unsigned int j = 0; j < edgeCount; j++)
        {
            unsigned int link = this->nodes[i].edge(j);
            std::cout << "(" << i << "," << link << ")  ";
        }
        std::cout << std::endl;
    }
}

bool graph::dfs(unsigned int curNode, uiVector &stack)
{
    if (this->nodes[curNode].color == COLOR_OF_NODE_GREY)
    {
        return true; // Цикл найден
    }
    if (this->nodes[curNode].color == COLOR_OF_NODE_BLACK)
    {
        return false;
    }

    this->nodes[curNode].color = COLOR_OF_NODE_GREY;

    unsigned int edgeCount = this->nodes[curNode].edgeSize();
    for (unsigned int i = 0; i < edgeCount; i++)
    {
        unsigned int link = this->nodes[curNode].edge(i);
        if (this->dfs(link, stack))
        {
            return true;
        }
    }

    stack.push_back(curNode);
    this->nodes[curNode].color = COLOR_OF_NODE_BLACK;
    return false;
}

bool graph::topological_sort(uiVector &result)
{
    uiVector stack;
    for (unsigned int i = 0; i < this->amount; i++)
    {
        if (this->nodes[i].color == 0)
        {
            if (dfs(i, stack))
            {
                return false; // Цикл найден
            }
        }
    }

    // Переворачиваем стек в правильный порядок
    std::reverse(stack.begin(), stack.end());
    result = stack;
    return true;
}

int main()
{
    graph g(5);
    g.print();

    uiVector sorted;
    if (g.topological_sort(sorted))
    {
        std::cout << "Topological order: ";
        for (unsigned int node : sorted)
        {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Graph contains a cycle!" << std::endl;
    }

    return 0;
}
