#include "timeline_handler.h"
#include "../services/Timeline_service.h"
#include "../services/Post.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace timeline_handler {
    crow::response handle_get_timeline(sqlite3* db, const crow::request& req) {
        std::string username;
        if (req.url_params.get("username")) {
            username = req.url_params.get("username");
        } else {
            auto body = crow::json::load(req.body);
            if (body && body["username"].t() == crow::json::type::String) {
                username = body["username"].s();
            } else {
                return crow::response(400, "Missing username");
            }
        }
        TimelineService service;
        auto posts = service.getTimelineForUser(db, username);
        json res = json::array();
        for (const auto& post : posts) {
            res.push_back({
                {"id", post.getId()},
                {"user_name", post.getUserName()},
                {"content", post.getContent()},
                {"image_url", post.getImageUrl()},
                {"timestamp", post.getcreated_at()},
                {"like_count", post.getLikeCount()},
                {"comment_count", post.getCommentCount()},
                {"share_count", post.getShareCount()}
            });
        }
        return crow::response(200, res.dump());
    }
}
