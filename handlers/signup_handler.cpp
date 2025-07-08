<<<<<<< HEAD
#include "../include/crow_all.h"
#include <sqlite3.h>

crow::response handleSignUp(sqlite3* db, const crow::request& req)
{
    
=======
#include "signup_handler.h"
#include "../utils/hash_utils.h"
// #include "../db/db_utils.h"  // Optional: or keep DB methods private here

// #include <sqlite3.h>

// extern sqlite3* db; // declared elsewhere

crow::response SignupHandler::handle_signup(const crow::request& req) {
    auto body = crow::json::load(req);
    if (!body)
        return crow::response(400, "Invalid JSON");

    std::string username = body["username"].s();
    std::string email = body["email"].s();
    std::string password = body["password"].s();

    if (username.empty() || email.empty() || password.empty())
        return crow::response(400, "Missing fields");

    if (is_email_taken(email))
        return crow::response(409, "Email already registered");

    std::string hashed_pw = hash_password(password);

    if (!insert_user(username, email, hashed_pw))
        return crow::response(500, "User creation failed");

    return crow::response(200, "Signup successful");
}

bool SignupHandler::is_email_taken(const std::string& email) {
    sqlite3_stmt* stmt;
    std::string query = "SELECT id FROM users WHERE email = ?";
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return exists;
}

bool SignupHandler::insert_user(const std::string& username, const std::string& email, const std::string& hashed_pw) {
    sqlite3_stmt* stmt;
    std::string query = "INSERT INTO users (username, email, password) VALUES (?, ?, ?)";
    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hashed_pw.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);

    sqlite3_finalize(stmt);
    return success;
}

// Optional (if you're using x-www-form-urlencoded instead of JSON)
std::unordered_map<std::string, std::string> SignupHandler::parse_form_data(const std::string& body) {
    std::unordered_map<std::string, std::string> formData;
    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        auto equalPos = pair.find('=');
        if (equalPos != std::string::npos) {
            std::string key = pair.substr(0, equalPos);
            std::string value = pair.substr(equalPos + 1);
            formData[key] = value;
        }
    }

    return formData;
>>>>>>> origin
}
