#include "friend_handler.h"
#include "../services/friend_search_service.h"
#include "../services/friend_suggestion_service.h"
#include "login_handler.h"  // for get_session_from_cookie, active_sessions

// All handlers proxy to FriendSearchService and enforce session

crow::response handleSearchUsers(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end())
        return crow::response(401, "Unauthorized");
    std::string username = active_sessions[session_id];
    FriendSearchService service(db);

    std::string query = req.url_params.get("q") ? req.url_params.get("q") : "";
    std::string type = req.url_params.get("type") ? req.url_params.get("type") : "username";
    int limit = req.url_params.get("limit") ? std::stoi(req.url_params.get("limit")) : 50;

    std::vector<SearchUser> results;
    if (type == "email") {
        results = service.searchUsersByEmail(query, username);
    } else if (query.empty()) {
        results = service.getAllUsers(username, limit);
    } else {
        results = service.searchUsersByPrefix(query, username, limit);
    }

    crow::json::wvalue res;
    res["users"] = service.usersToJson(results);
    res["count"] = (int)results.size();
    res["query"] = query;
    res["type"] = type;
    return crow::response(200, res);
}

crow::response handleGetFriends(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end())
        return crow::response(401, "Unauthorized");
    std::string username = active_sessions[session_id];
    FriendSearchService service(db);
    auto friends = service.getFriends(username);

    crow::json::wvalue res;
    res["friends"] = service.usersToJson(friends);
    res["count"] = (int)friends.size();
    return crow::response(200, res);
}

crow::response handleSendFriendRequest(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end())
        return crow::response(401, "Unauthorized");
    auto body = crow::json::load(req.body);
    if (!body || !body.has("target_username"))
        return crow::response(400, "Missing target_username");
    std::string requester = active_sessions[session_id];
    std::string addressee = body["target_username"].s();

    FriendSearchService service(db);
    if (service.sendFriendRequest(requester, addressee))
        return crow::response(200, R"({"success":true,"message":"Friend request sent"})");
    return crow::response(400, R"({"success":false,"message":"Failed to send friend request"})");
}

crow::response handleRespondToFriendRequest(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end())
        return crow::response(401, "Unauthorized");
    auto body = crow::json::load(req.body);
    if (!body || !body.has("requester_username") || !body.has("action"))
        return crow::response(400, "Missing requester_username or action");
    std::string addressee = active_sessions[session_id];
    std::string requester = body["requester_username"].s();
    std::string action = body["action"].s();

    FriendSearchService service(db);
    bool ok = false;
    if (action == "accept")
        ok = service.acceptFriendRequest(requester, addressee);
    else if (action == "reject")
        ok = service.rejectFriendRequest(requester, addressee);

    if (ok)
        return crow::response(200, R"({"success":true,"message":"Friend request processed"})");
    return crow::response(400, R"({"success":false,"message":"Failed to process friend request"})");
}

crow::response handleRemoveFriend(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end())
        return crow::response(401, "Unauthorized");
    auto body = crow::json::load(req.body);
    if (!body || !body.has("friend_username"))
        return crow::response(400, "Missing friend_username");
    std::string user1 = active_sessions[session_id];
    std::string user2 = body["friend_username"].s();

    FriendSearchService service(db);
    if (service.removeFriend(user1, user2))
        return crow::response(200, R"({"success":true,"message":"Friend removed"})");
    return crow::response(400, R"({"success":false,"message":"Failed to remove friend"})");
}

crow::response handleGetFriendSuggestions(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end())
        return crow::response(401, "Unauthorized");
    std::string username = active_sessions[session_id];
    int limit = req.url_params.get("limit") ? std::stoi(req.url_params.get("limit")) : 20;

    // Use AVL tree-based friend suggestion service
    FriendSuggestionService suggestionService(db);
    auto suggestions = suggestionService.suggestFriends(username, limit);



    // Convert to JSON format expected by frontend
    crow::json::wvalue res;
    crow::json::wvalue::list suggestionsList;
    
    for (const auto& suggestion : suggestions) {
        crow::json::wvalue userObj;
        userObj["username"] = suggestion.first;
        userObj["mutual_friends"] = suggestion.second;
        suggestionsList.push_back(std::move(userObj));
    }
    
    res["suggestions"] = std::move(suggestionsList);
    res["count"] = (int)suggestions.size();
    return crow::response(200, res);
}

crow::response handleGetFriendRequests(sqlite3* db, const crow::request& req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end())
        return crow::response(401, "Unauthorized");
    std::string username = active_sessions[session_id];

    FriendSearchService service(db);
    auto requests = service.getFriendRequests(username);

    crow::json::wvalue res;
    res["requests"] = service.usersToJson(requests);
    res["count"] = (int)requests.size();
    return crow::response(200, res);
}