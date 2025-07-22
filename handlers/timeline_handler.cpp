#include "timeline_handler.h"
#include "../services/dynamic_timeline_service.h"
#include "../services/Post.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace timeline_handler {
    crow::response handle_get_timeline(sqlite3* db, const crow::request& req) {
        std::string username;
        
        // Try to get username from URL parameters first
        if (req.url_params.get("username")) {
            username = req.url_params.get("username");
        } else {
            // Try to get from request body
            auto body = crow::json::load(req.body);
            if (body && body["username"].t() == crow::json::type::String) {
                username = body["username"].s();
            } else {
                return crow::response(400, "Missing username parameter");
            }
        }
        
        std::cout << "=== TIMELINE REQUEST FOR USER: " << username << " ===" << std::endl;
        
        // Use the new dynamic timeline service
        DynamicTimelineService dynamicService(db);
        auto posts = dynamicService.generateDynamicTimeline(username, 50);
        
        // Convert posts to JSON response
        json res = json::array();
        for (const auto& post : posts) {
            res.push_back({
                {"id", post.getId()},
                {"user_name", post.getUserName()},
                {"content", post.getContent()},
                {"image_url", post.getImageUrl()},
                {"timestamp", post.getTimestamp()},
                {"like_count", post.getLikeCount()},
                {"comment_count", post.getCommentCount()}
            });
        }
        
        std::cout << "Returning " << posts.size() << " posts in timeline" << std::endl;
        return crow::response(200, res.dump());
    }
}
