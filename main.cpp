#include "include/crow_all.h"
#include <unordered_map>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sqlite3.h>
#include "handlers/login_handler.h"
#include "handlers/signup_handler.h"
#include "handlers/JWT.h"


crow::response serve_file(const std::string& path, const std::string& content_type) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cout << "File not found: " << path << "\n";
        return crow::response(404, "File Not Found");
    }

    std::ostringstream contents;
    contents << file.rdbuf();

    std::string body = contents.str();
    std::cout << "Loaded file of size: " << body.size() << " bytes\n";

    crow::response res;
    res.set_header("Content-Type", content_type);
    res.write(body);
    return res;
}

int main()
{
    sqlite3 * db;

  if (sqlite3_open("../users.db", &db)) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return 1;
}

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (username TEXT UNIQUE, password TEXT);", nullptr, nullptr, nullptr);

    crow::SimpleApp app;
    

    CROW_ROUTE(app, "/")([]() {
        std::cout << "Serving index.html from: static/index.html\n";
        return serve_file("../static/index.html", "text/html");
    });
    
    CROW_ROUTE(app, "/style.css")([]() {
        return serve_file("../static/style.css", "text/css");
    });

    /*
    CROW_ROUTE(app, "/welcome")([]() {
        std::cout << "Serving welcome.html from: static/welcome.html\n";
        return serve_file("../static/welcome.html", "text/html");
    });
    */

    CROW_ROUTE(app, "/welcome").methods("GET"_method)([](const crow::request& req) {
        auto auth_header = req.get_header_value("Authorization");

        if (auth_header.substr(0, 7) != "Bearer ") {
            return crow::response(401, "Missing or malformed token");
        }

        std::string token = auth_header.substr(7);
        auto payload = JWT::verify_token(token);

        if (!payload.has_value()) {
            return crow::response(401, "Invalid or expired token");
        }

        std::string username = payload->email;
        // You could use this info to customize the response
        return crow::response(200, "Welcome " + username + "!");
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)([db](const crow::request& req){
        return handle_login(db,req);
    });

    
    CROW_ROUTE(app, "/<path>")
    ([](const std::string& path){
        std::cout << "Received unknown request: " << path << std::endl;
        return crow::response(404);
    });
    
    /*     CROW_ROUTE(app, "/favicon.ico")([](){
        return crow::response(204); // No Content
        });
    */

     
    CROW_ROUTE(app, "/signup").methods("POST"_method)([db](const crow::request& req){
        return handle_signup(db, req);
    });

    app.port(18080).multithreaded().run();
    sqlite3_close(db);
}
