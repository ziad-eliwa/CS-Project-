// post_service.h
#pragma once

#include <string>
#include <vector>
#include <sqlite3.h>

struct Post {
    int id;
    std::string user_name;
    std::string content;
    std::string image_url;
    std::string timestamp;
    std::string privacy;
};

class PostService {
public:
    static bool createPost(const std::string& user_name, const std::string& content,
                           const std::string& image_url, const std::string& privacy);

    static std::vector<Post> getAllPosts();
};
