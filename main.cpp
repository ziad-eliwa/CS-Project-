#include "include/crow_all.h"
<<<<<<< HEAD
#include <openssl/sha.h>
#include <sqlite3.h>
#include <bits/stdc++.h>
#include "handlers/login_handler.h"
#include "handlers/signup_handler.h"
#include "utils/db_utils.h"
=======
//#include <crow.h>
#include <unordered_map>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "handlers/login_handler.h"


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

>>>>>>> origin

int main()
{
    sqlite3 * db;

    sqlite3_open("users.db",&db);

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT);", nullptr, nullptr, nullptr);

    crow::SimpleApp app;
<<<<<<< HEAD

    CROW_ROUTE(app, "/login").methods("POST"_method)([db](const crow::request& req){
        return handleLogin(db, req);
    });

    CROW_ROUTE(app, "/signup").methods("POST"_method)([db](const crow::request& req){
       return handleSignUp(db, req); 
    });

    app.port(18080).multithreaded().run();
    sqlite3_close(db);
=======
    
    CROW_ROUTE(app, "/")([]() {
        std::cout << "Serving index.html from: static/index.html\n";
        return serve_file("static/index.html", "text/html");
    });
    
    CROW_ROUTE(app, "/style.css")([]() {
        return serve_file("static/style.css", "text/css");
    });

    CROW_ROUTE(app, "/welcome")([]() {
        std::cout << "Serving welcome.html from: static/welcome.html\n";
        return serve_file("static/welcome.html", "text/html");
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)([](const crow::request& req){
        return LoginHandler::handle_login(req);
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
/*     CROW_ROUTE(app, "/signup").methods("POST"_method)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if(!body) return crow::response(400,"Invalid JSON");

        return crow::response(200, "Signup successful");

    });
 */
    app.port(8080).multithreaded().run();
>>>>>>> origin
}