#include "Post.h"
#include <ctime>

// Default constructor
Post::Post() : id(0), user_name(""), content(""), image_url(""), created_at(0) {}

// Parameterized constructor
Post::Post(int id, const std::string& user_name, const std::string& content, const std::string& image_url, 
    std::time_t created_at, int like_count, int comment_count)
    : id(id), user_name(user_name), content(content), image_url(image_url),
      created_at(created_at), like_count(like_count), comment_count(comment_count) {}

// Minimal constructor for timeline fetch
Post::Post(int id, const std::string& content, const std::string& user_name, std::time_t created_at)
    : id(id), user_name(user_name), content(content), image_url(""), created_at(created_at), like_count(0), comment_count(0) {}

// Getters
int Post::getId() const {
    return id;
}

std::string Post::getUserName() const {
    return user_name;
}

std::string Post::getContent() const {
    return content;
}

std::string Post::getImageUrl() const {
    return image_url;
}


std::time_t Post::getcreated_at() const{
    return created_at;
}

int Post::getLikeCount() const {
    return like_count;
}

int Post::getCommentCount() const {
    return comment_count;
}

std::string Post::getTimestamp() const {
    char buf[32];
    std::tm* tm_info = std::localtime(&created_at);
    std::strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%S", tm_info);
    return std::string(buf);
}

// Setters

void Post::setId(int new_id) {
        id = new_id;
    }

void Post::setUserName(const std::string& new_user_name) {
    user_name = new_user_name;
    }

void Post::setContent(const std::string& new_content) {
        content = new_content;
    }
void Post::setImageUrl(const std::string& new_image_url) {
        image_url = new_image_url;
    }

void Post::setLikeCount(int count) {
    like_count = count;
}

void Post::setCommentCount(int count) {
    comment_count = count;
}

void Post::setCreatedAt(std::time_t createdAt) {
    created_at = createdAt;
}

void Post::setCreatedAt(const std::string& createdAt) {
    struct tm tm;
    if (strptime(createdAt.c_str(), "%Y-%m-%d %H:%M:%S", &tm)) {
        this->created_at = mktime(&tm);
    } else {
        this->created_at = 0;
    }
}