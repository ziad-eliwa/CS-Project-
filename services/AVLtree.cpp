#include "AVLtree.h"
#include <queue>

AVLTree::AVLTree() : root(nullptr) {}
AVLTree::~AVLTree() { destroy(root); }

void AVLTree::destroy(AVLNode* node) {
    if (!node) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

int AVLTree::getHeight(AVLNode* node) {
    return node ? node->height : 0;
}

int AVLTree::getBalance(AVLNode* node) {
    return node ? getHeight(node->left) - getHeight(node->right) : 0;
}

AVLNode* AVLTree::rotateRight(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    return x;
}

AVLNode* AVLTree::rotateLeft(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    return y;
}

AVLNode* AVLTree::insert(AVLNode* node, const std::string& key) {
    if (!node) return new AVLNode(key);
    if (key < node->key)
        node->left = insert(node->left, key);
    else if (key > node->key)
        node->right = insert(node->right, key);
    else
        return node;
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);
    if (balance > 1 && key < node->left->key)
        return rotateRight(node);
    if (balance < -1 && key > node->right->key)
        return rotateLeft(node);
    if (balance > 1 && key > node->left->key) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && key < node->right->key) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

void AVLTree::insert(const std::string& key) {
    root = insert(root, key);
}

AVLNode* AVLTree::minValueNode(AVLNode* node) {
    AVLNode* current = node;
    while (current && current->left)
        current = current->left;
    return current;
}

AVLNode* AVLTree::remove(AVLNode* node, const std::string& key) {
    if (!node) return node;
    if (key < node->key)
        node->left = remove(node->left, key);
    else if (key > node->key)
        node->right = remove(node->right, key);
    else {
        if (!node->left || !node->right) {
            AVLNode* temp = node->left ? node->left : node->right;
            if (!temp) {
                temp = node;
                node = nullptr;
            } else
                *node = *temp;
            delete temp;
        } else {
            AVLNode* temp = minValueNode(node->right);
            node->key = temp->key;
            node->right = remove(node->right, temp->key);
        }
    }
    if (!node) return node;
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    int balance = getBalance(node);
    if (balance > 1 && getBalance(node->left) >= 0)
        return rotateRight(node);
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0)
        return rotateLeft(node);
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    return node;
}

void AVLTree::remove(const std::string& key) {
    root = remove(root, key);
}

bool AVLTree::search(const std::string& key) {
    AVLNode* current = root;
    while (current) {
        if (key == current->key) return true;
        if (key < current->key) current = current->left;
        else current = current->right;
    }
    return false;
}

void AVLTree::inOrder(AVLNode* node, std::vector<std::string>& result) {
    if (!node) return;
    inOrder(node->left, result);
    result.push_back(node->key);
    inOrder(node->right, result);
}

std::vector<std::string> AVLTree::inOrder() {
    std::vector<std::string> result;
    inOrder(root, result);
    return result;
}

void AVLTree::levelOrder(AVLNode* node, std::vector<std::string>& result) {
    if (!node) return;
    std::queue<AVLNode*> q;
    q.push(node);
    while (!q.empty()) {
        AVLNode* curr = q.front(); q.pop();
        result.push_back(curr->key);
        if (curr->left) q.push(curr->left);
        if (curr->right) q.push(curr->right);
    }
}

std::vector<std::string> AVLTree::levelOrder() {
    std::vector<std::string> result;
    levelOrder(root, result);
    return result;
}
