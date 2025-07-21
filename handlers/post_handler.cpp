
// post_handler.cpp
#include "post_handler.h"
#include "../services/post_service.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

crow::response PostHandler::handleCreatePost(const crow::request& req) {
    try {
        auto body = json::parse(req.body);
        std::string user_name = body["user_name"];
        std::string content = body["content"];
        std::string image_url = body.value("image_url", "");
        std::string privacy = body.value("privacy", "Public");

        std::cerr << "[CreatePost] user_name: " << user_name << ", content: " << content << std::endl;
        int result = PostService::createPost(user_name, content, image_url);
        if (result == -1) {
            std::cerr << "[CreatePost] Failed: user not found or DB error." << std::endl;
            return crow::response(400, "Failed to create post: user not found or DB error");
        }
        return crow::response(201, "Post created successfully");
    } catch (const std::exception& e) {
        std::cerr << "[CreatePost] Exception: " << e.what() << std::endl;
        return crow::response(400, "Invalid JSON");
    }
}

crow::response PostHandler::handleGetAllPosts() {
    auto posts = PostService::getAllPosts();
    json res = json::array();
    for (const auto& post : posts) {
        res.push_back({
            {"id", post.getId()},
            {"user_name", post.getUserName()},
            {"content", post.getContent()},
            {"timestamp", std::to_string(post.getcreated_at())},
            {"like_count", post.getLikeCount()},
            {"comment_count", post.getCommentCount()}
        });
    }
    return crow::response(200, res.dump());
}

crow::response PostHandler::handleDeletePost(const crow::request& req) {
    try {
        auto body = json::parse(req.body);
        int post_id = body["post_id"];
        bool success = PostService::deletePost(post_id);
        if (success) {
            return crow::response(200, "Post deleted successfully");
        } else {
            return crow::response(400, "Failed to delete post");
        }
    } catch (const std::exception& e) {
        std::cerr << "[DeletePost] Exception: " << e.what() << std::endl;
        return crow::response(400, "Invalid JSON");
    }
}