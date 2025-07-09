// JWT.h
#pragma once

#include <string>
#include <unordered_map>
#include <optional>

struct TokenPayload {
    std::string email;
    std::string user_id;
};

class JWT {
public:
    static std::string generate_token(const std::string& email, const std::string& user_id);
    static std::optional<TokenPayload> verify_token(const std::string& token);
}; 
