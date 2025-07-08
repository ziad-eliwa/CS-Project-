//#include "include/crow_all.h"  //
#include <crow.h> //comment this line if you are using crow_all.h. I'm using this line because crow is installed globally on my machine. 
#include <unordered_map>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include "handlers/login_handler.h"
#include"handlers/signup_handler.h"

// * !IMPORTANT! THE ROUTE LINK IS http://localhost:8080/

crow::response serve_file(const std::string& path, const std::string& content_type) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cout << "File not found: " << path << "\n";
        return crow::response(404, "File Not Found");
    }
    // the above line is used to open the file in binary mode, which is important for serving files correctly.
    // It ensures that the file is read as a binary stream, which is necessary for non-text files.
    
    std::ostringstream contents;
    contents << file.rdbuf();
    // Read the entire file into a string stream
    
    std::string body = contents.str();
    std::cout << "Loaded file of size: " << body.size() << " bytes\n";
    // this line will excute in terminal when the file is loaded successfully. showing the size of the file loaded in bytes

    crow::response res;
    res.set_header("Content-Type", content_type);
    res.write(body);
    // 

    return res;
}


int main()
{
    crow::SimpleApp app;
    // this is the main application object for Crow, which will handle routing and requests.
    // always use this line to create the main application object for Crow.

    CROW_ROUTE(app, "/")([]() { // this is the root route, which executes the html file when loaded
        std::cout << "Serving index.html from: static/index.html\n";
        return serve_file("static/index.html", "text/html");
    });
    
    CROW_ROUTE(app, "/style.css")([]() { // this is the route for the css file, which is used to style the html file
        return serve_file("static/style.css", "text/css");
    });

    CROW_ROUTE(app, "/welcome")([]() { // this is the route for the welcome page, which will excute if and only if the user is logged in successfully. It also excute the welcome.html file.
        std::cout << "Serving welcome.html from: static/welcome.html\n";
        return serve_file("static/welcome.html", "text/html");
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)([](const crow::request& req){ // this is the route for the login page, which will excute when the user tries to login. wether the user is logged in successfully or not, it will always excute this route. and the excute method is POST. meaning that the entered data will be showen on the URL.
        return LoginHandler::handle_login(req);
    });

    CROW_ROUTE(app, "/<path>") // this is a catch-all route that will match any path not explicitly defined above, this is useful for handling unknown requests or serving static files. and if someone tries to access a path that is not defined, it will return a 404 response.
    ([](const std::string& path){
        std::cout << "Received unknown request: " << path << std::endl;
        return crow::response(404);
    });

/*     CROW_ROUTE(app, "/favicon.ico")([](){
        return crow::response(204); // No Content
    });

    CROW_ROUTE(app, "/signup").methods("POST"_method)([](const crow::request& req){
        auto body = crow::json::load(req.body);
        if(!body) return crow::response(400,"Invalid JSON");

        return crow::response(200, "Signup successful");

    });
 */
    app.port(8080).multithreaded().run();
    // the port is set to 8080, which can be accessed by the following link: http://localhost:8080/
}