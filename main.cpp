#include "include/crow_all.h"
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sqlite3.h>
#include "handlers/login_handler.h"
#include "handlers/signup_handler.h"

// Use the session management from login_handler
using ::active_sessions; // stores the session id->username
using ::get_session_from_cookie; //extracts session ID from HTTP cookies


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

  if (sqlite3_open("users.db", &db)) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return 1;
}

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (username TEXT UNIQUE, password TEXT);", nullptr, nullptr, nullptr);

    crow::SimpleApp app;
    

    CROW_ROUTE(app, "/")([]() {
        std::cout << "Serving index.html from: static/index.html\n";
        return serve_file("static/index.html", "text/html");
    });
    
    CROW_ROUTE(app, "/style.css")([]() {
        return serve_file("static/style.css", "text/css");
    });
    
    CROW_ROUTE(app, "/welcome")([](const crow::request& req) {
        std::string session_id = get_session_from_cookie(req);
        std::cout << "=== WELCOME PAGE ACCESS ATTEMPT ===" << std::endl;
        std::cout << "Session ID from cookie: '" << session_id << "'" << std::endl;
        std::cout << "Active sessions count: " << active_sessions.size() << std::endl;
        
        if (active_sessions.find(session_id) == active_sessions.end()) {
            // No valid session - redirect to login
            std::cout << "❌ INVALID SESSION - Redirecting to login" << std::endl;
            crow::response res(302);
            res.set_header("Location", "/");
            return res;
        }
        std::cout << "✅ VALID SESSION - Serving welcome page for user: " << active_sessions[session_id] << std::endl;
        return serve_file("static/welcome.html", "text/html");
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

    app.port(8080).multithreaded().run();
    sqlite3_close(db);
}


//                          to check cookies is working
/* Open browser Developer Tools (F12)
Go to http://localhost:8080/ and login normally
Once on welcome page, go to Application/Storage tab → Cookies
Find the session_id cookie and change its value to something random like "fake123"
Refresh the page
 */


                            // why this happens??

/*  get_session_from_cookie() returns "fake123"
"fake123" is not in active_sessions map
Invalid session = redirect to login
This proves our server validates sessions properly and doesn't trust client-side data
 */