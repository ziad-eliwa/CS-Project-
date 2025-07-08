#pragma once
//#include <crow.h>
#include "../include/crow_all.h"
#include <string>
#include <unordered_map>

class LoginHandler {
public:
    static crow::response handle_login(const crow::request& req);
    static bool validate_credentials(const std::string& email, const std::string& password);
    static std::unordered_map<std::string, std::string> parse_form_data(const std::string& body);
};
