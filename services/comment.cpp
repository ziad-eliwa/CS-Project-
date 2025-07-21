#include "Comment.h"
#include <ctime>
#include <string>
#include <iostream>


Comment::Comment() = default;

Comment::Comment(int id, int post_id, const std::string& user_name, const std::string& content, std::time_t created_at)
    : id(id), post_id(post_id), user_name(user_name), content(content), created_at(created_at) {}

// Getters
int Comment::getId() const {
    return id;
}

int Comment::getPostId() const {
    return post_id;
}

std::string Comment::getUserName() const {
    return user_name;
}

std::string Comment::getContent() const {
    return content;
}

std::time_t Comment::getCreatedAt() const {
    return created_at;
}

// Setters
void Comment::setContent(const std::string& new_content) {
    content = new_content;
}
void Comment::setCreatedAt(std::time_t new_created_at) {
    created_at = new_created_at;
}
void Comment::setId(int new_id) {
    id = new_id;
}
void Comment::setPostId(int new_post_id) {
    post_id = new_post_id;
}
void Comment::setUserName(const std::string& new_user_name) {
    user_name = new_user_name;
}

