#pragma once

#include <string>
#include <ctime>

class Comment {
public:
    int id;
    int post_id;
    std::string user_name;
    std::string content;
    std::time_t created_at;

    Comment();
    Comment(int id, int post_id, const std::string& user_name, const std::string& content, std::time_t created_at);

    // Getters
    int getId() const;
    int getPostId() const;
    std::string getUserName() const;
    std::string getContent() const;
    std::time_t getCreatedAt() const;

    // Setters
    void setContent(const std::string& new_content);
    void setCreatedAt(std::time_t new_created_at);
};