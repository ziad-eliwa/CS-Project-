#include "timeline_handler.h"
#include "../services/Timeline_service.h"
#include "../services/Post.h"
#include "login_handler.h" // for active_sessions, get_session_from_cookie
#include <string>

crow::response get_timeline(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
        return crow::response(401, "Unauthorized");
    }
    std::string username = active_sessions[session_id];
    TimelineService service;
    auto posts = service.getTimelineForUser(db, username);
    crow::json::wvalue result;
    result["posts"] = crow::json::wvalue::list();
    for (const auto& post : posts) {
        // You may need to adjust this if Post has more/less fields
        crow::json::wvalue post_json;
        // post_json["id"] = post.getId();
        // post_json["content"] = post.getContent();
        // post_json["author_id"] = post.getUserId();
        // post_json["like_count"] = post.getLikeCount();
        // post_json["comment_count"] = post.getCommentCount();
        // post_json["created_at"] = post.getCreationAt();
        result["posts"].push_back(post_json);
    }
    return crow::response(result);
}

crow::response add_post(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
        return crow::response(401, "Unauthorized");
    }
    std::string username = active_sessions[session_id];
    auto body = crow::json::load(req.body);
    if (!body || !body.has("content")) {
        return crow::response(400, "Missing content");
    }
    std::string content = body["content"].s();
    int postId = createPost(db, username, content);
    if (postId == -1) {
        return crow::response(500, "Failed to create post");
    }
    crow::json::wvalue result;
    result["success"] = true;
    result["post_id"] = postId;
    return crow::response(result);
}

crow::response delete_post(sqlite3* db, const crow::request& req, int postId) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
        return crow::response(401, "Unauthorized");
    }
    bool success = deletePost(db, postId);
    crow::json::wvalue result;
    result["success"] = success;
    return crow::response(result);
}
