#pragma once
#include <string>
#include <vector>
#include <ctime>
#include "Post.h"

struct PostAVLNode {
    Post post;
    PostAVLNode* left;
    PostAVLNode* right;
    int height;
    
    PostAVLNode(const Post& p) : post(p), left(nullptr), right(nullptr), height(1) {}
};

class PostAVLTree {
public:
    PostAVLTree();
    ~PostAVLTree();
    
    void insert(const Post& post);
    void remove(int postId);
    bool search(int postId);
    std::vector<Post> getTimelineInOrder(int limit = 50);  // Returns posts sorted by timestamp (newest first)
    std::vector<Post> getPostsByUser(const std::string& username, int limit = 50);
    void clear();
    int size() const;

private:
    PostAVLNode* root;
    int nodeCount;
    
    PostAVLNode* insert(PostAVLNode* node, const Post& post);
    PostAVLNode* remove(PostAVLNode* node, int postId);
    PostAVLNode* minValueNode(PostAVLNode* node);
    PostAVLNode* maxValueNode(PostAVLNode* node);
    
    int getHeight(PostAVLNode* node);
    int getBalance(PostAVLNode* node);
    PostAVLNode* rotateRight(PostAVLNode* y);
    PostAVLNode* rotateLeft(PostAVLNode* x);
    
    void reverseInOrder(PostAVLNode* node, std::vector<Post>& result, int& count, int limit);
    void collectPostsByUser(PostAVLNode* node, const std::string& username, std::vector<Post>& result, int& count, int limit);
    void destroy(PostAVLNode* node);
    
    bool isPostGreater(const Post& a, const Post& b);
    bool isPostEqual(const Post& a, const Post& b);
};
