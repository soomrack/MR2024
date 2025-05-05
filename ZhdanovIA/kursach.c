#include <iostream>
#include <map>
#include <string>
#include <vector>

class Node {
public:
    std::map<char, Node*> children;
    int start;
    int* end;  // Указатель для конца
    Node* suffix_link;

    Node(int start, int* end) : start(start), end(end), suffix_link(nullptr) {}
};

class SuffixTree {
private:
    Node* root;
    std::string text;
    int *leafEnd;
    int size;
    int activeNode;
    int activeEdge;
    int activeLength;

public:
    SuffixTree(const std::string &str) {
        text = str;
        size = str.size();
        leafEnd = new int(-1);
        root = new Node(-1, leafEnd);
        root->suffix_link = root;
        activeNode = 0;  // Индекс активного узла
        activeEdge = -1; // Индекс активного ребра
        activeLength = 0; // Длина активного ребра
        build();
    }

    ~SuffixTree() {
        delete root; // Указатель на корень освобождается; необходимо реализовать рекурсивное удаление
        delete leafEnd;
    }

    void build() {
        for (int i = 0; i < size; i++) {
            extend(i);
        }
    }

    void extend(int pos) {
        leafEnd = new int(pos); // Обновляем конец листа
        int lastCreatedNode = -1; // Узел, созданный на предыдущем шаге
        bool continueProcessing = true; // Флаг для продолжения обработки

        while (pos >= 0 && continueProcessing) {
            if (activeLength == 0) activeEdge = pos; // Если длина активного ребра 0, устанавливаем его на текущую позицию

            char currentChar = text[activeEdge]; // Текущий символ
            if (root->children.find(currentChar) == root->children.end()) {
                // Если символ не найден, создаем новое ребро
                Node* leafNode = new Node(pos, leafEnd);
                root->children[currentChar] = leafNode; // Добавляем новое ребро 
                if (lastCreatedNode != -1)
                    root->children[char(lastCreatedNode)]->suffix_link = root;

                lastCreatedNode = currentChar; // Обновляем последний созданный узел
            } else {
                Node* existingNode = root->children[currentChar];
                if (activeLength >= existingNode->end - existingNode->start) {
                    // Если длина активного ребра больше, чем длина существующего ребра
                    activeEdge += activeLength; // Переходим к следующему
                    activeLength = 0;
                    continue; // продолжаем с новым активным ребром
                }

                if (text[existingNode->start + activeLength] == text[pos]) {
                    // Если символ совпадает
                    if (lastCreatedNode != -1 && lastCreatedNode != currentChar) {
                        root->children[char(lastCreatedNode)]->suffix_link = existingNode; 
                        lastCreatedNode = -1;
                    }
                    activeLength++;
                } else {
                    // Если нет совпадения, создаем новое ребро
                    Node* newInternalNode = new Node(existingNode->start, new int(existingNode->start + activeLength));
                    root->children[currentChar] = newInternalNode; // Добавляем внутренний узел
                    newInternalNode->children[text[existingNode->start + activeLength]] = existingNode; // Присоединяем старый узел
                    existingNode->start += activeLength; // Обновляем состояние старого узла
                    newInternalNode->children[text[pos]] = new Node(pos, leafEnd); // Новое ребро для текущего символа

                    if (lastCreatedNode != -1)
                        root->children[char(lastCreatedNode)]->suffix_link = newInternalNode;

                    lastCreatedNode = currentChar;
                }

                if (activeNode == 0) {
                    if (activeLength == 0) {
                        continueProcessing = false; // Прекращаем обработку, если ничего не осталось
                    } else {
                        activeLength--; // уменьшаем длину
                        activeEdge = pos - 1; // устанавливаем обратно активный край
                    }
                }
            }
        }
    }
};

    void display(Node* node, int level = 0) {
        if (node == nullptr) return;

        for (auto &child : node->children) {
            for (int i = 0; i < level; i++)
                std::cout << "  ";
            std::cout << child.first << ": ";
            for (int i = child.second->start; i <= *(child.second->end); i++)
                std::cout << text[i];
            std::cout << std::endl;
            display(child.second, level + 1); // Рекурсивный вызов для дочернего узла
        }
    }

    void display() {
        display(root);
    }

int main() {
    std::string text = "MISSISSIPPI$";
    SuffixTree suffixTree(text);
    suffixTree.display(); // Выводим дерево
    return 0;
}
