#include "signup_handler.h"
#include <crow.h>
#include "../database/db_utils.h"
#include "../utils/hash_utils.h"
#include "../services/profile_services.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <sqlite3.h>


 crow::response handle_signup(sqlite3* db, const crow::request& req) {
    std::string content_type = req.get_header_value("Content-Type");
    std::string username, password;

    if (content_type.find("application/json") != std::string::npos) {
        auto body = crow::json::load(req.body);

        if (!body)
            return crow::response(400, "Invalid JSON");

        username = body["username"].s();
        password = body["password"].s();
    }
   else {
        return crow::response(415, "Unsupported Content-Type");
    }

    if (username.empty() || password.empty())
        return crow::response(400, "Missing fields");

    if (userExists(db, username)) {
        return crow::response(409, "User already exists");
    }

    if (registerUser(db, username, password)) {
        return crow::response(201, "Signup successful");
    } else {
        return crow::response(500, "Failed to register user");
    }
}