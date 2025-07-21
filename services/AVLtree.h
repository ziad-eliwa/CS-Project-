#pragma once
#include <string>
#include <vector>

struct AVLNode {
    std::string key;
    AVLNode* left;
    AVLNode* right;
    int height;
    AVLNode(const std::string& k) : key(k), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
public:
    AVLTree();
    ~AVLTree();
    void insert(const std::string& key);
    void remove(const std::string& key);
    bool search(const std::string& key);
    std::vector<std::string> inOrder();
    std::vector<std::string> levelOrder();
private:
    AVLNode* root;
    AVLNode* insert(AVLNode* node, const std::string& key);
    AVLNode* remove(AVLNode* node, const std::string& key);
    AVLNode* minValueNode(AVLNode* node);
    int getHeight(AVLNode* node);
    int getBalance(AVLNode* node);
    AVLNode* rotateRight(AVLNode* y);
    AVLNode* rotateLeft(AVLNode* x);
    void inOrder(AVLNode* node, std::vector<std::string>& result);
    void levelOrder(AVLNode* node, std::vector<std::string>& result);
    void destroy(AVLNode* node);
};
