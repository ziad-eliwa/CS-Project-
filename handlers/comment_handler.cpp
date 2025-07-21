#include <string>
#include <iostream>
#include "../services/comment_service.h"
#include "comment_handler.h"

// Example: Add a comment
void handleAddComment(const crow::request& req, crow::response& res) {
    int post_id = std::stoi(req.url_params.get("post_id"));
    std::string user_name = req.url_params.get("user_name");
    std::string content = req.url_params.get("content");
    int result = CommentService::createComment(nullptr, post_id, user_name, content); // Pass actual db pointer
    if (result != -1) {
        res.code = 200;
        res.write("Comment added successfully.");
    } else {
        res.code = 400;
        res.write("Failed to add comment.");
    }
    res.end();
}

// Example: Delete a comment
void handleDeleteComment(const crow::request& req, crow::response& res) {
    int comment_id = std::stoi(req.url_params.get("comment_id"));
    bool success = CommentService::deleteComment(nullptr, comment_id); // Pass actual db pointer
    if (success) {
        res.code = 200;
        res.write("Comment deleted successfully.");
    } else {
        res.code = 400;
        res.write("Failed to delete comment.");
    }
    res.end();
}

// Example: Get all comments for a post
void handleGetAllComments(const crow::request& req, crow::response& res) {
    int post_id = std::stoi(req.url_params.get("post_id"));
    auto comments = CommentService::getCommentsByPost(post_id);
    std::string result = "[";
    for (const auto& comment : comments) {
        result += "{\"id\":" + std::to_string(comment.getId()) + ",";
        result += "\"user_name\":\"" + comment.getUserName() + "\",";
        result += "\"content\":\"" + comment.getContent() + "\"},";
    }
    if (result.size() > 1) result.pop_back();
    result += "]";
    res.code = 200;
    res.write(result);
    res.end();
}
