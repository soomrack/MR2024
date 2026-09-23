#include <iostream>
#include <vector>
#include <string>
#include <cstdio>      
#include <openssl/sha.h>

using namespace std;

// Узел дерева Меркла
class MerkleNode {
public:
    MerkleNode* left;   
    MerkleNode* right;  
    string hash;        

    // Конструктор для листового узла
    MerkleNode(const std::string& data)
        : left(nullptr), right(nullptr), hash(sha256(data)) {
    }

    // Конструктор для родительского узла
    MerkleNode(MerkleNode* left, MerkleNode* right)
        : left(left), right(right) {
        
        std::string combinedHash = left->hash + (right ? right->hash : left->hash);
        hash = sha256(combinedHash);
    }

   
    ~MerkleNode() {
        delete left;
        delete right;
    }

private:
    
    static std::string sha256(const std::string& input) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, input.c_str(), input.size());
        SHA256_Final(hash, &sha256);

        char outputBuffer[SHA256_DIGEST_LENGTH * 2 + 1];
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
        }
        outputBuffer[SHA256_DIGEST_LENGTH * 2] = '\0';
        return std::string(outputBuffer);
    }
};

// Класс дерева Меркла
class MerkleTree {
public:
    MerkleNode* root;  

    // Конструктор
    MerkleTree(const vector<string>& data) {
        if (data.empty()) {
            root = nullptr;
            return;
        }

        vector<MerkleNode*> nodes;
        
        for (const auto& item : data) {
            nodes.push_back(new MerkleNode(item));
        }

        
        root = buildTree(nodes);
    }

    // Деструктор
    ~MerkleTree() {
        delete root;
    }

    // Вывод структуры дерева в консоль
    void printTree() const {
        if (!root) {
            cout << "Дерево пустое" << endl;
            return;
        }
        printNode(root, 0);
    }

    // Получение корневого хеша
    string getRootHash() const {
        if (!root) {
            return "empty";
        }
        return root->hash;
    }

private:
    // Рекурсивное построение очередного слоя дерева
    MerkleNode* buildTree(vector<MerkleNode*>& nodes) {
        if (nodes.size() == 1) {
            return nodes[0];
        }

        vector<MerkleNode*> newLevel;
        for (size_t i = 0; i < nodes.size(); i += 2) {
            MerkleNode* left = nodes[i];
           
            MerkleNode* right = (i + 1 < nodes.size()) ? nodes[i + 1] : nullptr;

            MerkleNode* parent = new MerkleNode(left, right);
            newLevel.push_back(parent);
        }

        return buildTree(newLevel);
    }

    // Рекурсивный вывод узла и его потомков с отступами 
    void printNode(MerkleNode* node, int depth) const {
        if (!node) return;

        string indent(depth * 2, ' ');
        cout << indent << "Уровень " << depth << ": " << node->hash << endl;

        printNode(node->left, depth + 1);
        printNode(node->right, depth + 1);
    }
};

int main() {
    // Пример данных
    vector<string> data = {
        "1 string",
        "2 string",
        "3 string",
        "4 string",
        "5 string"
    };

    MerkleTree tree(data);

    cout << "Структура дерева Меркла:" << endl;
    tree.printTree();

    cout << "\nКорневой хеш: " << tree.getRootHash() << endl;

    return 0;
}