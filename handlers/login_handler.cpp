<<<<<<< HEAD
#include "../include/crow_all.h"
#include <sqlite3.h>

crow::response handleLogin(sqlite3* db, const crow::request& req)
{

=======
#include "login_handler.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>

// For now, we'll use a simple in-memory storage for demo users
// In a real application, this would be a database
static std::unordered_map<std::string, std::string> users = {
    {"user@example.com", "password123"},
    {"admin@test.com", "admin123"},
    {"john@email.com", "mypassword"}
};

crow::response LoginHandler::handle_login(const crow::request& req) {
    std::cout << "Login request received\n";
    std::cout << "Request body: " << req.body << "\n";
    
    // Parse form data
    auto form_data = parse_form_data(req.body);
    
    // Check if email and password are provided
    if (form_data.find("email") == form_data.end() || 
        form_data.find("password") == form_data.end()) {
        std::cout << "Missing email or password\n";
        return crow::response(400, "Email and password are required");
    }
    
    std::string email = form_data["email"];
    std::string password = form_data["password"];
    
    std::cout << "Parsed email: '" << email << "'\n";
    std::cout << "Parsed password: '" << password << "'\n";
    
    // Check if fields are not empty
    if (email.empty() || password.empty()) {
        std::cout << "Empty email or password\n";
        return crow::response(400, "Email and password cannot be empty");
    }
    
    // Validate credentials
    if (validate_credentials(email, password)) {
        std::cout << "Login successful for: " << email << "\n";
        crow::response res(200);
        res.set_header("Content-Type", "application/json");
        res.write("{\"success\": true, \"message\": \"Login successful!\", \"redirect\": \"/welcome?login=success\"}");
        return res;
    } else {
        std::cout << "Login failed for: " << email << "\n";
        return crow::response(401, "Invalid email or password");
    }
}

bool LoginHandler::validate_credentials(const std::string& email, const std::string& password) {
    auto it = users.find(email);
    if (it != users.end() && it->second == password) {
        return true;
    }
    return false;
}

std::unordered_map<std::string, std::string> LoginHandler::parse_form_data(const std::string& body) {
    std::unordered_map<std::string, std::string> form_data;
    std::istringstream stream(body);
    std::string pair;
    
    while (std::getline(stream, pair, '&')) {
        auto equals_pos = pair.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = pair.substr(0, equals_pos);
            std::string value = pair.substr(equals_pos + 1);
            
            // URL decode the value
            std::string decoded_value;
            for (size_t i = 0; i < value.length(); ++i) {
                if (value[i] == '+') {
                    decoded_value += ' ';
                } else if (value[i] == '%' && i + 2 < value.length()) {
                    // Convert hex to char
                    std::string hex = value.substr(i + 1, 2);
                    char ch = static_cast<char>(std::stoi(hex, nullptr, 16));
                    decoded_value += ch;
                    i += 2; // Skip the next 2 characters
                } else {
                    decoded_value += value[i];
                }
            }
            
            form_data[key] = decoded_value;
        }
    }
    
    return form_data;
>>>>>>> origin
}
