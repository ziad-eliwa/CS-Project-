// post_service.h
#pragma once

#include "Post.h"
#include <string>
#include <vector>
#include <sqlite3.h>

class PostService {
public: 

    // Retrieve a post by its ID
    static Post* getPostById(int post_id);

    // Get all posts by a user
    std::vector<Post> getPostsByUser(sqlite3* db, const std::string& username);

    // Get all posts
    static std::vector<Post> getAllPosts();

    // Update a post's content, image, or privacy
    static bool updatePost(int post_id, const std::string& new_content,
                          const std::string& new_image_url, const std::string& new_privacy);

    // Delete a post by its ID
    static bool deletePost(int post_id);
    // Create a new post
    static int createPost(const std::string& username, const std::string& content, const std::string& image_url);


    static bool PostService::deletePost(int post_id);
};
