// post_service.h
#pragma once

#include "Post.h"
#include <string>
#include <vector>
#include <sqlite3.h>
#include "database/db_utils.h"

class PostService {
public: 

    // Retrieve a post by its ID
    static Post* getPostById(int post_id);

    // Update a post's content, image, or privacy
    static bool updatePost(int post_id, const std::string& new_content,
                          const std::string& new_image_url, const std::string& new_privacy);
    
};
