#include <iostream>
#include <vector>
#include <string>
#include <openssl/sha.h>

class MerkleNode {
public:
    std::string hash;
    MerkleNode* left;
    MerkleNode* right;

    // Конструктор для листового узла
    MerkleNode(const std::string& h) : hash(h), left(nullptr), right(nullptr) {}

    // Конструктор для внутреннего узла
    MerkleNode(MerkleNode* l, MerkleNode* r) : left(l), right(r) {
        hash = computeHash(l->hash + r->hash);
    }

private:
    // Функция вычисления SHA-256 хеша
    std::string computeHash(const std::string& data) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((unsigned char*)data.c_str(), data.size(), hash);
        std::string hashStr;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            char buf[3];
            sprintf(buf, "%02x", hash[i]);
            hashStr += buf;
        }
        return hashStr;
    }
};

// Функция построения дерева Меркла из набора блоков данных
MerkleNode* buildMerkleTree(const std::vector<std::string>& dataBlocks) {
    if (dataBlocks.empty()) return nullptr;

    std::vector<MerkleNode*> nodes;
    // Создаем листовые узлы
    for (const auto& block : dataBlocks) {
        nodes.push_back(new MerkleNode(computeHash(block)));
    }

    // Строим дерево снизу вверх
    while (nodes.size() > 1) {
        std::vector<MerkleNode*> newLevel;
        for (size_t i = 0; i < nodes.size(); i += 2) {
            MerkleNode* left = nodes[i];
            // Если узлов нечетное количество, дублируем последний узел
            MerkleNode* right = (i + 1 < nodes.size()) ? nodes[i + 1] : left;
            MerkleNode* parent = new MerkleNode(left, right);
            newLevel.push_back(parent);
        }
        nodes = newLevel;
    }
    return nodes[0];
}

// Функция вычисления хеша для блока данных (вспомогательная)
std::string computeHash(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)data.c_str(), data.size(), hash);
    std::string hashStr;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        char buf[3];
        sprintf(buf, "%02x", hash[i]);
        hashStr += buf;
    }
    return hashStr;
}

// Функция проверки целостности данных
bool verifyIntegrity(const std::vector<std::string>& dataBlocks, const std::string& rootHash) {
    MerkleNode* root = buildMerkleTree(dataBlocks);
    if (!root) return false;
    return root->hash == rootHash;
}

// Пример работы алгоритма
int main() {
    // Набор данных
    std::vector<std::string> data = {"data1", "data2", "data3", "data4"};

    // Построение дерева и получение корневого хеша
    MerkleNode* root = buildMerkleTree(data);
    std::string rootHash = root->hash;
    std::cout << "Корневой хеш: " << rootHash << std::endl;

    // Проверка целостности исходных данных
    bool isValid = verifyIntegrity(data, rootHash);
    std::cout << "Целостность данных: " << (isValid ? "Подтверждена" : "Нарушена") << std::endl;

    // Изменение данных
    std::vector<std::string> modifiedData = data;
    modifiedData[2] = "data3_modified";

    // Проверка целостности после изменения
    isValid = verifyIntegrity(modifiedData, rootHash);
    std::cout << "Целостность после изменения: " << (isValid ? "Подтверждена" : "Нарушена") << std::endl;

    return 0;
}