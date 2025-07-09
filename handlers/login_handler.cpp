#include "login_handler.h"
#include "../include/crow_all.h"
#include "../utils/db_utils.h"
#include "../utils/hash_utils.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <sqlite3.h>

std::unordered_map<std::string, std::string> parseform_data(const std::string& body) {
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
        return crow::response(200, "Login successful");
    } else {
        return crow::response(401, "Invalid username or password");
    }
}
