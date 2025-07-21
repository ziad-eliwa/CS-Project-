// post_handler.cpp
#include "post_handler.h"
#include "post_service.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

crow::response PostHandler::handleCreatePost(const crow::request& req) {
    try {
        auto body = json::parse(req.body);
        std::string user_name = body["user_name"];
        std::string content = body["content"];
        std::string image_url = body.value("image_url", "");
        std::string privacy = body.value("privacy", "Public");

        bool success = PostService::createPost(user_name, content, image_url, privacy);
        if (success)
            return crow::response(201, "Post created successfully");
        else
            return crow::response(500, "Failed to create post");
    } catch (const std::exception& e) {
        return crow::response(400, "Invalid JSON");
    }
}

crow::response PostHandler::handleGetAllPosts() {
    auto posts = PostService::getAllPosts();
    json res = json::array();

    for (const auto& post : posts) {
        res.push_back({
            {"id", post.id},
            {"user_name", post.user_name},
            {"content", post.content},
            {"image_url", post.image_url},
            {"timestamp", post.timestamp},
            {"privacy", post.privacy}
        });
    }

    return crow::response(200, res.dump());
}
