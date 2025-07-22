#include "PostAVLTree.h"
#include <algorithm>

PostAVLTree::PostAVLTree() : root(nullptr), nodeCount(0) {}

PostAVLTree::~PostAVLTree() {
    destroy(root);
}

void PostAVLTree::insert(const Post& post) {
    root = insert(root, post);
    nodeCount++;
}

void PostAVLTree::remove(int postId) {
    root = remove(root, postId);
    nodeCount--;
}

bool PostAVLTree::search(int postId) {
    PostAVLNode* current = root;
    while (current != nullptr) {
        if (current->post.getId() == postId) {
            return true;
        } else if (postId < current->post.getId()) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return false;
}

std::vector<Post> PostAVLTree::getTimelineInOrder(int limit) {
    std::vector<Post> result;
    int count = 0;
    reverseInOrder(root, result, count, limit);
    return result;
}

std::vector<Post> PostAVLTree::getPostsByUser(const std::string& username, int limit) {
    std::vector<Post> result;
    int count = 0;
    collectPostsByUser(root, username, result, count, limit);
    
    std::sort(result.begin(), result.end(), [](const Post& a, const Post& b) {
        return a.getcreated_at() > b.getcreated_at();
    });
    
    return result;
}

void PostAVLTree::clear() {
    destroy(root);
    root = nullptr;
    nodeCount = 0;
}

int PostAVLTree::size() const {
    return nodeCount;
}

PostAVLNode* PostAVLTree::insert(PostAVLNode* node, const Post& post) {
    if (node == nullptr) {
        return new PostAVLNode(post);
    }
    
    if (isPostGreater(post, node->post)) {
        node->right = insert(node->right, post);
    } else if (!isPostEqual(post, node->post)) {
        node->left = insert(node->left, post);
    } else {
        node->post = post;
        return node;
    }
    
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    
    int balance = getBalance(node);
    
    if (balance > 1 && !isPostGreater(post, node->left->post)) {
        return rotateRight(node);
    }
    
    if (balance < -1 && isPostGreater(post, node->right->post)) {
        return rotateLeft(node);
    }
    
    if (balance > 1 && isPostGreater(post, node->left->post)) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    
    if (balance < -1 && !isPostGreater(post, node->right->post)) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    
    return node;
}

PostAVLNode* PostAVLTree::remove(PostAVLNode* node, int postId) {
    if (node == nullptr) {
        return node;
    }
    
    if (postId < node->post.getId()) {
        node->left = remove(node->left, postId);
    } else if (postId > node->post.getId()) {
        node->right = remove(node->right, postId);
    } else {
        if (node->left == nullptr || node->right == nullptr) {
            PostAVLNode* temp = node->left ? node->left : node->right;
            
            if (temp == nullptr) {
                temp = node;
                node = nullptr;
            } else {
                *node = *temp;
            }
            delete temp;
        } else {
            PostAVLNode* temp = minValueNode(node->right);
            node->post = temp->post;
            node->right = remove(node->right, temp->post.getId());
        }
    }
    
    if (node == nullptr) {
        return node;
    }
    
    node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
    
    int balance = getBalance(node);
    
    if (balance > 1 && getBalance(node->left) >= 0) {
        return rotateRight(node);
    }
    
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    
    if (balance < -1 && getBalance(node->right) <= 0) {
        return rotateLeft(node);
    }
    
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    
    return node;
}

PostAVLNode* PostAVLTree::minValueNode(PostAVLNode* node) {
    PostAVLNode* current = node;
    while (current->left != nullptr) {
        current = current->left;
    }
    return current;
}

PostAVLNode* PostAVLTree::maxValueNode(PostAVLNode* node) {
    PostAVLNode* current = node;
    while (current->right != nullptr) {
        current = current->right;
    }
    return current;
}

int PostAVLTree::getHeight(PostAVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return node->height;
}

int PostAVLTree::getBalance(PostAVLNode* node) {
    if (node == nullptr) {
        return 0;
    }
    return getHeight(node->left) - getHeight(node->right);
}

PostAVLNode* PostAVLTree::rotateRight(PostAVLNode* y) {
    PostAVLNode* x = y->left;
    PostAVLNode* T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    
    return x;
}

PostAVLNode* PostAVLTree::rotateLeft(PostAVLNode* x) {
    PostAVLNode* y = x->right;
    PostAVLNode* T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = std::max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = std::max(getHeight(y->left), getHeight(y->right)) + 1;
    
    return y;
}

void PostAVLTree::reverseInOrder(PostAVLNode* node, std::vector<Post>& result, int& count, int limit) {
    if (node == nullptr || count >= limit) {
        return;
    }
    
    reverseInOrder(node->right, result, count, limit);
    
    if (count < limit) {
        result.push_back(node->post);
        count++;
    }
    
    reverseInOrder(node->left, result, count, limit);
}

void PostAVLTree::collectPostsByUser(PostAVLNode* node, const std::string& username, std::vector<Post>& result, int& count, int limit) {
    if (node == nullptr || count >= limit) {
        return;
    }
    collectPostsByUser(node->left, username, result, count, limit);
    if (count < limit && node->post.getUserName() == username) {
        result.push_back(node->post);
        count++;
    }
    collectPostsByUser(node->right, username, result, count, limit);
}

void PostAVLTree::destroy(PostAVLNode* node) {
    if (node != nullptr) {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

bool PostAVLTree::isPostGreater(const Post& a, const Post& b) {
    if (a.getcreated_at() != b.getcreated_at()) {
        return a.getcreated_at() > b.getcreated_at();
    }
    return a.getId() > b.getId();
}

bool PostAVLTree::isPostEqual(const Post& a, const Post& b) {
    return a.getId() == b.getId();
}
