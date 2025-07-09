// JWT.cpp
#include "JWT.h"
#include <jwt-cpp/jwt.h>
#include <chrono>
#include <iostream>

static const std::string SECRET_KEY = "YourSuperSecretKeyChangeMe";

std::string JWT::generate_token(const std::string& email, const std::string& user_id) {
    using namespace std::chrono;

    // Time now and expiration
    auto now = system_clock::now();
    auto exp = now + minutes(30);

    // Create token
    std::string token = jwt::create()
        .set_type("JWT")
        .set_issued_at(now)
        .set_expires_at(exp)
        .set_payload_claim("email", jwt::claim(email))
        .set_payload_claim("user_id", jwt::claim(user_id))
        .sign(jwt::algorithm::hs256{SECRET_KEY});

    return token;
}

std::optional<TokenPayload> JWT::verify_token(const std::string& token) {
    try {
        // Decode token
        auto decoded = jwt::decode(token);

        // Create verifier with the same secret
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{SECRET_KEY})
            .with_claim("email", jwt::claim_type::string)
            .with_claim("user_id", jwt::claim_type::string);

        // Verify signature & expiration
        verifier.verify(decoded);

        // Check expiration manually (optional)
        /*
        auto exp = decoded.get_expires_at();
        if (std::chrono::system_clock::now() > exp) {
            return std::nullopt;
        }
        */

        TokenPayload payload;
        payload.email = decoded.get_payload_claim("email").as_string();
        payload.user_id = decoded.get_payload_claim("user_id").as_string();
        return payload;

    } catch (const std::exception& e) {
        std::cerr << "Token verification failed: " << e.what() << std::endl;
        return std::nullopt;
    }
}
