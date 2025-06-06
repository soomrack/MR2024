#include <iostream>
#include <vector>
#include <string>
#include <algorithm>  
#include <openssl/sha.h>


//#include <functional> 


using namespace std;


// Merkle tree
class MerkleNode {
public:
    MerkleNode* left;
    MerkleNode* right;
    string hash;

    //Input leaf
    MerkleNode(const std::string& data)
        : left(nullptr), right(nullptr), hash(sha256(data)) {}

    //Input node
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
    // SHA-256
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

// Build tree
class MerkleTree {
public:
    MerkleNode* root;
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

    ~MerkleTree() {
        delete root;
    }

    // Output Tree
    void printTree() const {
        if (!root) {
           cout << "Tree is empty" << endl;
            return;
        }

        printNode(root, 0);
    }

    
    string getRootHash() const {
        return root ? root->hash : "";
    }

private:    
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

    // Output
    void printNode(MerkleNode* node, int depth) const {
        if (!node) return;

        string indent(depth * 2, ' ');
        cout << indent << "Level " << depth << ": " << node->hash << endl;

        printNode(node->left, depth + 1);
        printNode(node->right, depth + 1);
    }
};

int main() {
    // Åxample of 5 elements
    vector<string> data = {
        "1 string",
        "2 string",
        "3 string",
        "4 string",
        "5 string" 
    };

    MerkleTree tree(data);

    cout << "Merkle Tree Structure:" << endl;
    tree.printTree();

    cout << "\nRoot Hash: " << tree.getRootHash() << endl;

    return 0;
}


/*
    // Input leaf
    MerkleNode(const string& data) : left(nullptr), right(nullptr), hash(calculateHash(data)) {}

    // Input Node
    MerkleNode(MerkleNode* left, MerkleNode* right): left(left), right(right) {

      string combinedHash = left->hash + (right ? right->hash : "");
        hash = calculateHash(combinedHash);
    }

    ~MerkleNode() {
        delete left;
        delete right;
    }

private:
    static string calculateHash(const string& input) {
        std::hash<string> hasher;
        return to_string(hasher(input));
    }
};
*/