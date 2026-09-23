#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <iomanip>
#include <sstream>
#include <openssl/evp.h>

class MerkleNode {
public:
    std::unique_ptr<MerkleNode> left;
    std::unique_ptr<MerkleNode> right;
    std::string hash;

    // Листовой узел
    explicit MerkleNode(const std::string& data)
        : hash(sha256(data)) {
    }

    // Родительский узел
    MerkleNode(std::unique_ptr<MerkleNode> leftNode,
        std::unique_ptr<MerkleNode> rightNode)
        : left(std::move(leftNode)),
        right(std::move(rightNode))
    {
        const std::string& rightHash =
            right ? right->hash : left->hash;

        hash = sha256(left->hash + rightHash);
    }

private:
    static std::string sha256(const std::string& input)
    {
        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int digestLength = 0;

        EVP_MD_CTX* context = EVP_MD_CTX_new();

        if (!context) {
            throw std::runtime_error("Не удалось создать EVP_MD_CTX");
        }

        if (EVP_DigestInit_ex(context, EVP_sha256(), nullptr) != 1 ||
            EVP_DigestUpdate(context, input.data(), input.size()) != 1 ||
            EVP_DigestFinal_ex(context, digest, &digestLength) != 1)
        {
            EVP_MD_CTX_free(context);
            throw std::runtime_error("Ошибка вычисления SHA-256");
        }

        EVP_MD_CTX_free(context);

        std::ostringstream result;

        for (unsigned int i = 0; i < digestLength; ++i) {
            result << std::hex
                << std::setw(2)
                << std::setfill('0')
                << static_cast<int>(digest[i]);
        }

        return result.str();
    }
};


class MerkleTree {
private:
    std::unique_ptr<MerkleNode> root;

public:
    explicit MerkleTree(const std::vector<std::string>& data)
    {
        if (data.empty()) {
            return;
        }

        std::vector<std::unique_ptr<MerkleNode>> nodes;

        nodes.reserve(data.size());

        for (const auto& item : data) {
            nodes.push_back(
                std::make_unique<MerkleNode>(item)
            );
        }

        buildTree(nodes);
        root = std::move(nodes[0]);
    }

    // Запрещаем копирование
    MerkleTree(const MerkleTree&) = delete;
    MerkleTree& operator=(const MerkleTree&) = delete;

    // Перемещение разрешаем
    MerkleTree(MerkleTree&&) noexcept = default;
    MerkleTree& operator=(MerkleTree&&) noexcept = default;

    ~MerkleTree() = default;


    std::string getRootHash() const
    {
        if (!root) {
            return "";
        }

        return root->hash;
    }


    void printTree() const
    {
        if (!root) {
            std::cout << "Дерево пустое\n";
            return;
        }

        printNode(root.get(), 0, "Root");
    }


private:

    static void buildTree(
        std::vector<std::unique_ptr<MerkleNode>>& nodes)
    {
        while (nodes.size() > 1)
        {
            std::vector<std::unique_ptr<MerkleNode>> newLevel;

            newLevel.reserve((nodes.size() + 1) / 2);

            for (std::size_t i = 0; i < nodes.size(); i += 2)
            {
                auto left = std::move(nodes[i]);

                std::unique_ptr<MerkleNode> right;

                if (i + 1 < nodes.size()) {
                    right = std::move(nodes[i + 1]);
                }

                newLevel.push_back(
                    std::make_unique<MerkleNode>(
                        std::move(left),
                        std::move(right)
                    )
                );
            }

            nodes = std::move(newLevel);
        }
    }


    static void printNode(
        const MerkleNode* node,
        int depth,
        const std::string& name)
    {
        if (!node) {
            return;
        }

        std::string indent(depth * 2, ' ');

        std::cout
            << indent
            << name
            << ": "
            << node->hash
            << '\n';

        printNode(node->left.get(), depth + 1, "Left");
        printNode(node->right.get(), depth + 1, "Right");
    }
};


int main()
{
    std::vector<std::string> data = {
        "1 string",
        "2 string",
        "3 string",
        "4 string",
        "5 string"
    };

    MerkleTree tree(data);

    std::cout << "Структура дерева Меркла:\n";
    tree.printTree();

    std::cout << "\nКорневой хеш:\n";
    std::cout << tree.getRootHash() << '\n';

    return 0;
}