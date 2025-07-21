#pragma once

#include <string>
#include <ctime>

class Comment {

    int id;
    int post_id;
    std::string user_name;
    std::string content;
    std::time_t created_at;

    public:

    Comment();
    Comment(int id, int post_id, const std::string& user_name, const std::string& content, std::time_t created_at);

    // Getters
    int getId() const;
    int getPostId() const;
    std::string getUserName() const;
    std::string getContent() const;
    std::time_t getCreatedAt() const;
    std::string getTimestamp() const;

    // Setters
    void setContent(const std::string& new_content);
    void setId(int new_id);
    void setPostId(int new_post_id);
    void setUserName(const std::string& new_user_name);
    void setCreatedAt(std::time_t new_created_at);
};