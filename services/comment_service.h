#pragma once

#include "Comment.h"
#include <string>
#include <vector>
#include <sqlite3.h>

class CommentService {
public:

    // Get all comments for a post
    static std::vector<Comment> getCommentsByPost(int post_id);

    // Get a comment by its ID
    static Comment* getCommentById(int comment_id);

    // Update a comment's content
    static bool updateComment(int comment_id, const std::string& new_content);
};
