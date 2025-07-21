#pragma once

#include "Comment.h"
#include "Post.h"
#include <string>
#include <vector>
#include <sqlite3.h>

class CommentService {
public:

    // Create a new comment on a post
    static int createComment(sqlite3* db, int postId, const std::string& username, const std::string& comment);

    // Update a comment's content
    static bool updateComment(int comment_id, const std::string& new_content);

    // Delete a comment by its ID
    static bool deleteComment(sqlite3* db, int commentId);

    // Get all comments for a post
    static std::vector<Comment> getCommentsByPost(int post_id);

    // Get a comment by its ID
    static Comment* getCommentById(int comment_id);

    // Update a comment's content
    static bool updateComment(int comment_id, const std::string& new_content);
};
