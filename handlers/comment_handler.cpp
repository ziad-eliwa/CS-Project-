#include <string>
#include <iostream>
#include "../services/comment_service.h"
#include "comment_handler.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

crow::response handle_create_comment(sqlite3* db, const crow::request& req) {
    try {
        std::cout << "RAW BODY: " << req.body << std::endl;
        auto body = json::parse(req.body);
        int post_id;
        if (body["post_id"].is_number()) {
            post_id = body["post_id"];
        } else if (body["post_id"].is_string()) {
            post_id = std::stoi(body["post_id"].get<std::string>());
        } else {
            return crow::response(400, "Invalid post_id type");
        }
        std::string user_name = body["user_name"];
        std::string content = body["content"];
        int result = CommentService::createComment(db, post_id, user_name, content);
        if (result != -1) {
            return crow::response(201, "Comment added successfully.");
        } else {
            return crow::response(400, "Failed to add comment.");
        }
    } catch (const std::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return crow::response(400, "Invalid JSON");
    }
}

crow::response handle_get_comments(sqlite3* db, const crow::request& req) {
    auto url_post_id = req.url_params.get("post_id");
    if (!url_post_id) {
        return crow::response(400, "Missing post_id");
    }
    int post_id = std::stoi(url_post_id);
    auto comments = CommentService::getCommentsByPost(post_id);
    json res = json::array();
    for (const auto& comment : comments) {
        res.push_back({
            {"id", comment.getId()},
            {"user_name", comment.getUserName()},
            {"content", comment.getContent()},
            {"created_at", comment.getTimestamp()}
        });
    }
    return crow::response(200, res.dump());
}
