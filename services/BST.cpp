#include "BST.h"

BST::BST() : root(nullptr) {}
BST::~BST() { destroy(root); }

void BST::destroy(BSTNode* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

BSTNode* BST::insert(BSTNode* node, const std::string& key) {
    if (!node) return new BSTNode(key);
    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    return node;
}

void BST::insert(const std::string& key) {
    root = insert(root, key);
}

bool BST::search(BSTNode* node, const std::string& key) {
    if (!node) return false;
    if (key == node->key) return true;
    if (key < node->key) return search(node->left, key);
    else return search(node->right, key);
}

bool BST::search(const std::string& key) {
    return search(root, key);
}

void BST::inOrder(BSTNode* node, std::vector<std::string>& result) {
    if (!node) return;
    inOrder(node->left, result);
    result.push_back(node->key);
    inOrder(node->right, result);
}

std::vector<std::string> BST::inOrder() {
    std::vector<std::string> result;
    inOrder(root, result);
    return result;
}

void BST::prefixSearch(BSTNode* node, const std::string& prefix, std::vector<std::string>& result) {
    if (!node) return;
    if (node->key.compare(0, prefix.size(), prefix) == 0)
        result.push_back(node->key);
    if (prefix <= node->key)
        prefixSearch(node->left, prefix, result);
    if (prefix >= node->key)
        prefixSearch(node->right, prefix, result);
}

std::vector<std::string> BST::prefixSearch(const std::string& prefix) {
    std::vector<std::string> result;
    prefixSearch(root, prefix, result);
    return result;
}
