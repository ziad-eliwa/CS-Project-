#ifndef POST_H
#define POST_H
#include <string>
#include <ctime>
#include <iostream>
#include <vector>
#include "Comment.h"

class Post {

    int id;
    std::string user_name;
    std::string content;
    std::string image_url;
    std::time_t created_at;

public:

    int like_count=0;
    int comment_count=0;

    std::vector <Comment> comments;


    Post();
    Post(int id, const std::string& user_name, const std::string& content, const std::string& image_url, 
        std::time_t created_at, int like_count, int comment_count = 0);
    // Minimal constructor for timeline fetch
    Post(int id, const std::string& content, const std::string& user_name, std::time_t created_at);

    // Getters
    int getId() const;
    std::string getUserName() const;
    std::string getContent() const;
    std::string getImageUrl() const;
    std::string getTimestamp() const;
    std::string getPrivacy() const;
    std::time_t getcreated_at() const;
    int getLikeCount() const;
    int getCommentCount() const;

    // Setters
    void setId(int new_id);
    void setUserName(const std::string& new_user_name);
    void setContent(const std::string& new_content);
    void setImageUrl(const std::string& new_image_url);
    void setTimestamp(const std::string& new_timestamp);
    void setPrivacy(const std::string& new_privacy);
    void setLikeCount(int count);
    void setCommentCount(int count);
    void setCreatedAt(const std::string& createdAt);
    void setCreatedAt(std::time_t createdAt); // new overload
};

#endif