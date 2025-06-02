#include <iostream>
#include <vector>
#include <string>
#include <sstream>

std::string simpleHash(const std::string& data) {
    unsigned int sum = 0;
    for (char c : data) {
        sum += static_cast<unsigned int>(c);
    }
    return std::to_string(sum);
}

class MerkleNode {
public:
    std::string hash;
    MerkleNode* left;
    MerkleNode* right;

    MerkleNode(const std::string& data) : left(nullptr), right(nullptr) {
        hash = simpleHash(data);
    }

    MerkleNode(MerkleNode* left, MerkleNode* right) : left(left), right(right) {
        hash = simpleHash(left->hash + right->hash);
    }
};

MerkleNode* buildMerkleTree(std::vector<std::string> leaves) {
    if (leaves.size() % 2 != 0) {
        leaves.push_back(leaves.back());
    }

    std::vector<MerkleNode*> nodes;
    for (const auto& leaf : leaves) {
        nodes.push_back(new MerkleNode(leaf));
    }

    while (nodes.size() > 1) {
        if (nodes.size() % 2 != 0) {
            nodes.push_back(nodes.back());
        }
        std::vector<MerkleNode*> parents;
        for (size_t i = 0; i < nodes.size(); i += 2) {
            parents.push_back(new MerkleNode(nodes[i], nodes[i + 1]));
        }
        nodes = parents;
    }

    return nodes.front();
}

int main() {
    std::vector<std::string> data = {
        "transaction1",
        "transaction2",
        "transaction3",
        "transaction4",
        "transaction5",
        "transaction6"
    };

    MerkleNode* root = buildMerkleTree(data);
    std::cout << "Merkle Root (simple hash): " << root->hash << std::endl;

    return 0;
}
