#pragma once
#include "crow.h"
#include <unordered_map>
#include <string>

class SignupHandler {
public:
    static crow::response handle_signup(const crow::request& req);
    static std::unordered_map<std::string, std::string> parse_form_data(const std::string& body);
private:
    static bool is_email_taken(const std::string& email);
    static bool insert_user(const std::string& username, const std::string& email, const std::string& hashed_pw);
};
