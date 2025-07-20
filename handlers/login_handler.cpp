#include "login_handler.h"
#include "../include/crow_all.h"
#include "../utils/db_utils.h"
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

std::unordered_map<std::string, std::string> parseform_data(const std::string& body) {
    // Converts "username=john&password=1234" to {"john", "1234"}
    
    std::unordered_map <std::string, std::string> form_data;
    std::istringstream stream(body);
    std::string pair;

    while (std::getline(stream, pair, '&')) {
        auto equals_pos = pair.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = pair.substr(0, equals_pos);
            std::string value = pair.substr(equals_pos + 1);

            std::string decoded_value;
            for (size_t i = 0; i < value.length(); ++i) {
                if (value[i] == '+') {
                    decoded_value += ' ';
                } else if (value[i] == '%' && i + 2 < value.length()) {
                    std::string hex = value.substr(i + 1, 2);
                    char ch = static_cast<char>(std::stoi(hex, nullptr, 16));
                    decoded_value += ch;
                    i += 2;
                } else {
                    decoded_value += value[i];
                }
            }

            form_data[key] = decoded_value;
        }
    }

    return form_data;
}

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
    else if (content_type.find("application/x-www-form-urlencoded") != std::string::npos) {
        auto form = parseform_data(req.body);
        username = form["username"];
        password = form["password"];
    }
    else {
        return crow::response(415, "Unsupported Content-Type");
    }

    if (username.empty() || password.empty())
        return crow::response(400, "Missing fields");


    if (checkCredentials(db, username, password)) {
        // Generate session ID and store it
        std::string session_id = generate_session_id();
        active_sessions[session_id] = username;
        
        // Create JSON response with redirect and set cookie
        crow::response res(200);
        res.set_header("Content-Type", "application/json");
        res.set_header("Set-Cookie", "session_id=" + session_id + "; Path=/; HttpOnly");
        res.write("{\"success\": true, \"message\": \"Login successful!\", \"redirect\": \"/welcome\"}");
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
    
    std::string session_id = "sess_";
    for (int i = 0; i < 32; ++i) {
        int random_num = dis(gen);
        session_id += (random_num < 10) ? ('0' + random_num) : ('a' + random_num - 10);
    }
    return session_id;
}

// Extract session ID from cookie
std::string get_session_from_cookie(const crow::request& req) {
    auto cookie_header = req.get_header_value("Cookie");
    if (cookie_header.empty()) {
        return "";
    }
    
    std::string session_prefix = "session_id=";
    size_t pos = cookie_header.find(session_prefix);
    if (pos == std::string::npos) {
        return "";
    }
    
    pos += session_prefix.length();
    size_t end_pos = cookie_header.find(';', pos);
    if (end_pos == std::string::npos) {
        end_pos = cookie_header.length();
    }
    
    return cookie_header.substr(pos, end_pos - pos);
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