#include "like_handler.h"
#include "../services/like_service.h"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

crow::response handleToggleLike(sqlite3* db, const crow::request& req, const std::string& username) {
    try {
        auto body = json::parse(req.body);
        if (!body.contains("post_id")) {
            return crow::response(400, "Missing post_id");
        }
        
        int post_id;
        if (body["post_id"].is_number()) {
            post_id = body["post_id"];
        } else if (body["post_id"].is_string()) {
            post_id = std::stoi(body["post_id"].get<std::string>());
        } else {
            return crow::response(400, "Invalid post_id type");
        }
        
        bool success = LikeService::toggleLike(db, post_id, username);
        if (success) {
            bool is_liked = LikeService::isPostLikedByUser(db, post_id, username);
            json response = {
                {"success", true},
                {"liked", is_liked},
                {"message", is_liked ? "Post liked" : "Post unliked"}
            };
            return crow::response(200, response.dump());
        } else {
            return crow::response(400, "Failed to toggle like");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in handleToggleLike: " << e.what() << std::endl;
        return crow::response(400, "Invalid JSON");
    }
}

crow::response handleGetLikeStatus(sqlite3* db, const crow::request& req, const std::string& username) {
    auto post_id_param = req.url_params.get("post_id");
    if (!post_id_param) {
        return crow::response(400, "Missing post_id parameter");
    }
    
    try {
        int post_id = std::stoi(post_id_param);
        bool is_liked = LikeService::isPostLikedByUser(db, post_id, username);
        
        json response = {
            {"liked", is_liked},
            {"post_id", post_id}
        };
        return crow::response(200, response.dump());
    } catch (const std::exception& e) {
        return crow::response(400, "Invalid post_id");
    }
}