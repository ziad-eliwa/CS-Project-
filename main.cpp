#include "handlers/login_handler.h"
#include "handlers/post_handler.h"
#include "handlers/profile_handler.h"
#include "handlers/signup_handler.h"
#include "handlers/timeline_handler.h"
#include <crow.h>
#include <filesystem>
#include <fstream>
#include <sqlite3.h>
#include <sstream>
#include <unordered_map>
#include "handlers/comment_handler.h"
#include "handlers/friend_handler.h"
#include "handlers/like_handler.h"
#include "handlers/search_handler.h"

// Use the session management from login_handler
using ::active_sessions;         // stores the session id->username
using ::get_session_from_cookie; // extracts session ID from HTTP cookies

crow::response serve_file(const std::string &path,
                          const std::string &content_type) {
  // Get the executable's directory and go up to project root
  std::filesystem::path exe_path = std::filesystem::current_path();
  std::filesystem::path project_root = exe_path;

  // If we're in build/Debug, go up two levels to project root
  if (project_root.filename() == "Debug" &&
      project_root.parent_path().filename() == "build") {
    project_root = project_root.parent_path().parent_path();
  }
  // If we're just in build, go up one level
  else if (project_root.filename() == "build") {
    project_root = project_root.parent_path();
  }

  std::filesystem::path full_path = project_root / path;
  std::ifstream file(full_path, std::ios::binary);
  if (!file) {
    std::cout << "File not found: " << full_path << "\n";
    std::cout << "Current working directory: " << exe_path << "\n";
    std::cout << "Project root: " << project_root << "\n";
    return crow::response(404, "File Not Found");
  }

  std::ostringstream contents;
  contents << file.rdbuf();

  std::string body = contents.str();
  std::cout << "Loaded file of size: " << body.size()
            << " bytes from: " << full_path << "\n";

  crow::response res;
  res.set_header("Content-Type", content_type);
  res.write(body);
  return res;
}

void initialize_schema(sqlite3 *db) {
  std::ifstream schema_file("../database/schema.sql");
  std::stringstream buffer;
  buffer << schema_file.rdbuf();
  std::string schema_sql = buffer.str();
  char *errMsg = nullptr;
  if (sqlite3_exec(db, schema_sql.c_str(), nullptr, nullptr, &errMsg) !=
      SQLITE_OK) {
    std::cerr << "Schema initialization failed: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }
}

int main() {
  
  sqlite3 *db;

  if (sqlite3_open("../database/users.db", &db)) {
    std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
    return 1;
  }
  initialize_schema(db);
  crow::SimpleApp app;

  CROW_ROUTE(app, "/")
  ([](const crow::request &req) {
    std::cout << "Serving index.html from: /static/index.html\n";
    // Log session info for debugging
    std::string session_id = get_session_from_cookie(req);
    if (!session_id.empty() && active_sessions.find(session_id) != active_sessions.end()) {
      std::cout << "User already logged in: " << active_sessions[session_id] << std::endl;
    }
    return serve_file("static/index.html", "text/html");
  });

  CROW_ROUTE(app, "/style.css")
  ([]() { return serve_file("static/style.css", "text/css"); });

  CROW_ROUTE(app, "/timeline.css")
  ([]() { return serve_file("static/timeline.css", "text/css"); });

  CROW_ROUTE(app, "/timeline.js")
  ([]() { return serve_file("static/timeline.js", "application/javascript"); });

  CROW_ROUTE(app, "/friends")
  ([](const crow::request &req) {
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
      crow::response res(302);
      res.set_header("Location", "/");
      return res;
    }
    return serve_file("static/friends.html", "text/html");
  });

  CROW_ROUTE(app, "/friends.css")
  ([]() { return serve_file("static/friends.css", "text/css"); });

  CROW_ROUTE(app, "/friends.js")
  ([]() { return serve_file("static/friends.js", "application/javascript"); });

  CROW_ROUTE(app, "/timeline")
  ([](const crow::request &req) {
    std::string session_id = get_session_from_cookie(req);
    std::cout << "=== TIMELINE PAGE ACCESS ATTEMPT ===" << std::endl;
    std::cout << "Session ID from cookie: '" << session_id << "'" << std::endl;
    std::cout << "Active sessions count: " << active_sessions.size()
              << std::endl;

    if (active_sessions.find(session_id) == active_sessions.end()) {
      std::cout << "INVALID SESSION - Redirecting to login" << std::endl;
      crow::response res(302);
      res.set_header("Location", "/");
      return res;
    }
    std::cout << "VALID SESSION - Serving timeline page for user: "
              << active_sessions[session_id] << std::endl;
    return serve_file("static/timeline.html", "text/html");
  });

  CROW_ROUTE(app, "/welcome")
  ([]() { return serve_file("static/welcome.html", "text/html"); });

  CROW_ROUTE(app, "/login")
      .methods("POST"_method)(
          [db](const crow::request &req) { return handle_login(db, req); });

  CROW_ROUTE(app, "/api/timeline")
      .methods("GET"_method, "POST"_method)([db](const crow::request &req) {
        // Session validation for timeline access
        std::string session_id = get_session_from_cookie(req);
        if (active_sessions.find(session_id) == active_sessions.end()) {
          return crow::response(401, "Unauthorized");
        }
        std::string username = active_sessions[session_id];
        
        // Create a new request with username in body for the handler
        crow::request modified_req = req;
        std::string body_with_username = "{\"username\":\"" + username + "\"}";
        modified_req.body = body_with_username;
        
        return timeline_handler::handle_get_timeline(db, modified_req);
      });

  CROW_ROUTE(app, "/api/posts").methods("GET"_method)([](const crow::request &req) {
    // Session validation for getting posts
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
      return crow::response(401, "Unauthorized");
    }
    return PostHandler::handleGetAllPosts();
  });

  CROW_ROUTE(app, "/api/posts")
      .methods("POST"_method)([](const crow::request &req) {
        // Session validation for creating posts
        std::string session_id = get_session_from_cookie(req);
        if (active_sessions.find(session_id) == active_sessions.end()) {
          return crow::response(401, "Unauthorized");
        }
        // Add username from session to request context
        std::string username = active_sessions[session_id];
        return PostHandler::handleCreatePost(req, username);
      });

  CROW_ROUTE(app, "/api/comments").methods("POST"_method)([db](const crow::request& req) {
    // Session validation for creating comments
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
      return crow::response(401, "Unauthorized");
    }
    std::string username = active_sessions[session_id];
    return handle_create_comment(db, req, username);
  });
  CROW_ROUTE(app, "/api/comments").methods("GET"_method)([db](const crow::request& req) {
    // Session validation for getting comments
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
      return crow::response(401, "Unauthorized");
    }
    return handle_get_comments(db, req);
  });

  // Friend Management API Routes
  CROW_ROUTE(app, "/api/friends/search").methods("GET"_method)([db](const crow::request& req) {
    return handleSearchUsers(db, req);
  });

  // Search posts endpoint
  CROW_ROUTE(app, "/api/search/posts").methods("GET"_method)([db](const crow::request& req) {
    return search_handler::handle_search_posts(db, req);
  });

  CROW_ROUTE(app, "/api/friends").methods("GET"_method)([db](const crow::request& req) {
    return handleGetFriends(db, req);
  });

  CROW_ROUTE(app, "/api/friends/request").methods("POST"_method)([db](const crow::request& req) {
    return handleSendFriendRequest(db, req);
  });

  CROW_ROUTE(app, "/api/friends/respond").methods("POST"_method)([db](const crow::request& req) {
    return handleRespondToFriendRequest(db, req);
  });

  CROW_ROUTE(app, "/api/friends/remove").methods("POST"_method)([db](const crow::request& req) {
    return handleRemoveFriend(db, req);
  });

  CROW_ROUTE(app, "/api/friends/suggestions").methods("GET"_method)([db](const crow::request& req) {
    return handleGetFriendSuggestions(db, req);
  });

  CROW_ROUTE(app, "/api/friends/requests").methods("GET"_method)([db](const crow::request& req) {
    return handleGetFriendRequests(db, req);
  });

  // Like Management API Routes
  CROW_ROUTE(app, "/api/likes/toggle").methods("POST"_method)([db](const crow::request& req) {
    // Session validation for toggling likes
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
      return crow::response(401, "Unauthorized");
    }
    std::string username = active_sessions[session_id];
    return handleToggleLike(db, req, username);
  });

  CROW_ROUTE(app, "/api/likes/status").methods("GET"_method)([db](const crow::request& req) {
    // Session validation for getting like status
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
      return crow::response(401, "Unauthorized");
    }
    std::string username = active_sessions[session_id];
    return handleGetLikeStatus(db, req, username);
  });

  // Get current user endpoint
  CROW_ROUTE(app, "/api/user/current")
      .methods("GET"_method)([](const crow::request &req) {
        std::string session_id = get_session_from_cookie(req);
        if (!session_id.empty() && active_sessions.find(session_id) != active_sessions.end()) {
          std::string username = active_sessions[session_id];
          return crow::response(200, "{\"success\": true, \"username\": \"" + username + "\"}");
        }
        return crow::response(401, "{\"success\": false, \"message\": \"Not authenticated\"}");
      });

  // Logout endpoint
  CROW_ROUTE(app, "/api/logout")
      .methods("POST"_method)([](const crow::request &req) {
        std::string session_id = get_session_from_cookie(req);
        if (!session_id.empty() && active_sessions.find(session_id) != active_sessions.end()) {
          active_sessions.erase(session_id);
          std::cout << "User logged out, session removed: " << session_id << std::endl;
          return crow::response(200, "{\"success\": true, \"message\": \"Logged out successfully\"}");
        }
        return crow::response(400, "{\"success\": false, \"message\": \"No active session found\"}");
      });

  CROW_ROUTE(app, "/signup")
      .methods("POST"_method)(
          [db](const crow::request &req) { return handle_signup(db, req); });

  CROW_ROUTE(app, "/<path>")
  ([](const std::string &path) {
    std::cout << "Received unknown request: " << path << std::endl;
    return crow::response(404);
  });

  /*     CROW_ROUTE(app, "/favicon.ico")([](){
      return crow::response(204); // No Content
      });
  */

  app.port(18080).multithreaded().run();
  sqlite3_close(db);

    
}

//                          to check cookies is working
/* Open browser Developer Tools (F12)
Go to http://localhost:8080/ and login normally
Once on welcome page, go to Application/Storage tab → Cookies
Find the session_id cookie and change its value to something random like
"fake123" Refresh the page
 */

// why this happens??

/*  get_session_from_cookie() returns "fake123"
"fake123" is not in active_sessions map
Invalid session = redirect to login
This proves our server validates sessions properly and doesn't trust client-side
data
 */