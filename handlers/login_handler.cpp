#include "login_handler.h"
#include <crow.h>
#include "../database/db_utils.h"
#include "../utils/hash_utils.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <sqlite3.h>
#include <random>
#include <chrono>

// Global session storage
std::unordered_map<std::string, std::string> active_sessions; //session_id->username

crow::response handle_login(sqlite3* db, const crow::request& req) {
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


    if (checkCredentials(db, username, password)) {
        // Generate session ID and store it
        std::string session_id = generate_session_id(); // generate ID if the credintials are correct
        active_sessions[session_id] = username; // mapping it to the username
        
        // Create JSON response with redirect and set cookie
        crow::response res(200);
        res.set_header("Content-Type", "application/json");
        // setts the cookie in the users browser
        res.set_header("Set-Cookie", "session_id=" + session_id + "; Path=/; HttpOnly");
        // sends message to the front end that the login is successful
        res.write("{\"success\": true, \"message\": \"Login successful!\", \"redirect\": \"/timeline\"}"); 
        return res;
    } else {
        return crow::response(401, "Invalid username or password");
    }
}

// Generate random session ID
std::string generate_session_id() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::string session_id = "sess_"; // for debugging and database
    for (int i = 0; i < 32; ++i) {
        int random_num = dis(gen);
        session_id += (random_num < 10) ? ('0' + random_num) : ('a' + random_num - 10);
    }
    return session_id;
}

// Extract session ID from cookie
std::string get_session_from_cookie(const crow::request& req) {
    // searches for the cookie header in the HTTP request
    auto cookie_header = req.get_header_value("Cookie");
    if (cookie_header.empty()) {
        return "";
    }
    
    // return empty string if the session_id feild is not found in the cookie header
    std::string session_prefix = "session_id=";
    size_t pos = cookie_header.find(session_prefix);
    if (pos == std::string::npos) {
        return "";
    }
    
    pos += session_prefix.length(); // skips the session_id string and looks at the value
    size_t end_pos = cookie_header.find(';', pos); // looks for the next semi-colon
    if (end_pos == std::string::npos) {
        end_pos = cookie_header.length(); // if its not found then it assumes the whole string is the session_id value
    }
    
    return cookie_header.substr(pos, end_pos - pos); // return substring between the two values ( gives only the session ID value)
}
/* 1. User Submits Login Form
Browser sends: POST /login with form data username=john&password=secret123
2. handle_login() Processes Request
Extract data: username="john", password="secret123"
Check database: Is john's password really "secret123"?
If valid: Generate session_id="sess_abc123def456"
Store session: active_sessions["sess_abc123def456"] = "john"
Send cookie: Tell browser to remember "session_id=sess_abc123def456"
Send response: JSON with redirect to "/welcome"
3. Browser Redirects to /welcome
Browser goes to: GET /welcome
Browser includes: Cookie: session_id=sess_abc123def456
4. Welcome Route Checks Session
Extract cookie: get_session_from_cookie() returns "sess_abc123def456"
Check sessions: Is "sess_abc123def456" in active_sessions?
Found: Yes! "sess_abc123def456" -> "john"
Result: Show welcome page
 */