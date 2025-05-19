#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

#define SHA256_HEX_LENGTH (SHA256_DIGEST_LENGTH * 2 + 1)

// Узел дерева Меркла с хеш-суммой и ссылками на потомков
typedef struct MerkleNode {
    char hash[SHA256_HEX_LENGTH];
    struct MerkleNode* left;
    struct MerkleNode* right;
} MerkleNode;

// Прототипы внутренних функций
static void compute_hash(const char* data, char* output);
static MerkleNode* create_leaf(const char* data);
static MerkleNode* build_parent(MerkleNode* left, MerkleNode* right);
static MerkleNode* build_merkle_layer(MerkleNode** nodes, int count);

// Основной интерфейс
MerkleNode* build_merkle_tree(const char** data, int count);
void destroy_merkle_tree(MerkleNode* root);
const char* get_merkle_root(const MerkleNode* root);

// Вычисляет SHA-256 и сохраняет в виде HEX-строки
static void compute_hash(const char* data, char* output) {
    unsigned char raw[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)data, strlen(data), raw);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", raw[i]);
    }
    output[SHA256_HEX_LENGTH - 1] = '\0';
}

// Создает конечный узел с хешем данных
static MerkleNode* create_leaf(const char* data) {
    MerkleNode* node = (MerkleNode*)malloc(sizeof(MerkleNode));
    if (!node) return NULL;

    compute_hash(data, node->hash);
    node->left = node->right = NULL;
    return node;
}

// Создает родительский узел из двух дочерних (дублирует хеш при нечетном числе)
static MerkleNode* build_parent(MerkleNode* left, MerkleNode* right) {
    MerkleNode* parent = (MerkleNode*)malloc(sizeof(MerkleNode));
    if (!parent) return NULL;

    char combined[SHA256_HEX_LENGTH * 2 + 1] = {0};
    const char* right_hash = right ? right->hash : left->hash;
    
    snprintf(combined, sizeof(combined), "%s%s", left->hash, right_hash);
    compute_hash(combined, parent->hash);
    
    parent->left = left;
    parent->right = right;
    return parent;
}

// Рекурсивно строит слои дерева до корня
static MerkleNode* build_merkle_layer(MerkleNode** nodes, int count) {
    if (count == 1) return nodes[0];

    int new_count = (count + 1) / 2;
    MerkleNode** parents = (MerkleNode**)calloc(new_count, sizeof(MerkleNode*));
    if (!parents) return NULL;

    for (int i = 0; i < new_count; i++) {
        int left_idx = 2 * i;
        int right_idx = (left_idx + 1 < count) ? left_idx + 1 : left_idx;
        
        parents[i] = build_parent(nodes[left_idx], nodes[right_idx]);
        if (!parents[i]) {
            for (int j = 0; j < i; j++) free(parents[j]);
            free(parents);
            return NULL;
        }
    }

    MerkleNode* root = build_merkle_layer(parents, new_count);
    free(parents);
    return root;
}

// Публичный интерфейс: строит дерево из массива строк
MerkleNode* build_merkle_tree(const char** data, int count) {
    if (count == 0) return NULL;

    MerkleNode** leaves = (MerkleNode**)calloc(count, sizeof(MerkleNode*));
    if (!leaves) return NULL;

    for (int i = 0; i < count; i++) {
        leaves[i] = create_leaf(data[i]);
        if (!leaves[i]) {
            for (int j = 0; j < i; j++) destroy_merkle_tree(leaves[j]);
            free(leaves);
            return NULL;
        }
    }

    MerkleNode* root = build_merkle_layer(leaves, count);
    free(leaves);
    return root;
}

// Рекурсивно освобождает память дерева
void destroy_merkle_tree(MerkleNode* root) {
    if (!root) return;
    destroy_merkle_tree(root->left);
    destroy_merkle_tree(root->right);
    free(root);
}

// Возвращает корневой хеш дерева
const char* get_merkle_root(const MerkleNode* root) {
    return root ? root->hash : NULL;
}

// Пример использования
int main() {
    const char* data[] = {"Alice", "Bob", "Charlie", "Diana"};
    const int count = sizeof(data)/sizeof(data[0]);

    MerkleNode* tree = build_merkle_tree(data, count);
    if (!tree) {
        fprintf(stderr, "Ошибка построения дерева\n");
        return 1;
    }

    printf("Merkle Root: %s\n", get_merkle_root(tree));
    destroy_merkle_tree(tree);
    return 0;
}
