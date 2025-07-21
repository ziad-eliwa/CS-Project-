#include "profile_handler.h"
#include "../services/profile_services.h"
#include "../include/crow_all.h"
#include <sqlite3.h>
#include <nlohmann/json.hpp>

extern std::map<std::string, std::string> active_sessions; // session_id -> username
extern std::string get_session_from_cookie(const crow::request& req);

crow::response handle_get_profile(sqlite3* db, const crow::request& req) {

    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.count(session_id) == 0) {
        return crow::response(401, "Not logged in");
    }
    std::string username = active_sessions[session_id];

    UserProfile profile;
    if (!ProfileService::getProfile(db, username, profile)) {
        return crow::response(404, "Profile not found");
    }

    nlohmann::json j;
    j["display_name"] = profile.display_name;
    j["avatar_url"]   = profile.avatar_url;
    j["bio"]          = profile.bio;
    j["location"]     = profile.location;
    j["website"]      = profile.website;
    return crow::response(200, j.dump());
}

crow::response handle_update_profile(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.count(session_id) == 0) {
        return crow::response(401, "Not logged in");
    }
    std::string username = active_sessions[session_id];

    nlohmann::json j;
    try {
        j = nlohmann::json::parse(req.body);
    } catch (...) {
        return crow::response(400, "Invalid JSON");
    }


    UserProfile update;
    update.display_name = j.value("display_name", "");
    update.avatar_url   = j.value("avatar_url", "");
    update.bio          = j.value("bio", "");
    update.location     = j.value("location", "");
    update.website      = j.value("website", "");

    if (ProfileService::updateProfile(db, username, update))
        return crow::response(200, "Profile updated");
    else
        return crow::response(500, "Update failed");
}
