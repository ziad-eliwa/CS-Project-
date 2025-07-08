#pragma once
#include <crow.h> // comment this if you will be using the header file of crow
//#include "../include/crow_all.h"
#include <string>
#include <unordered_map> // using map because of the examples credintials (dummy emails and passwords)

class LoginHandler {
public:
    // I'm using static functions becuase it does not need to create obj of the class and instead uses the functions directly. But the class offers function management and organization.
    
    static crow::response handle_login(const crow::request& req);
    //validate_credentials function compares the entered email and passowrd with the ones in the unsorted map
    static bool validate_credentials(const std::string& email, const std::string& password);

    static std::unordered_map<std::string, std::string> parse_form_data(const std::string& body);
};
