#pragma once
#include <string>
#include <vector>

struct BSTNode {
    std::string key;
    BSTNode* left;
    BSTNode* right;
    BSTNode(const std::string& k) : key(k), left(nullptr), right(nullptr) {}
};

class BST {
public:
    BST();
    ~BST();
    void insert(const std::string& key);
    bool search(const std::string& key);
    std::vector<std::string> inOrder();
    std::vector<std::string> prefixSearch(const std::string& prefix);
private:
    BSTNode* root;
    BSTNode* insert(BSTNode* node, const std::string& key);
    bool search(BSTNode* node, const std::string& key);
    void inOrder(BSTNode* node, std::vector<std::string>& result);
    void prefixSearch(BSTNode* node, const std::string& prefix, std::vector<std::string>& result);
    void destroy(BSTNode* node);
};
