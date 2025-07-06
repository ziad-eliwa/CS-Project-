#include "include/crow_all.h"
#include <unordered_map>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

int main()
{
    crow::SimpleApp app;

    CROW_ROUTE(app, "/login").methods("POST"_method)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if(!body) return crow::response(400,"Invalid JSON");
        
        return crow::response(200, "login successful");
    });

    CROW_ROUTE(app, "/signup").methods("POST"_method)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if(!body) return crow::response(400,"Invalid JSON");

        return crow::response(200, "Signup successful");

    });

    app.port(18080).multithreaded().run();
}