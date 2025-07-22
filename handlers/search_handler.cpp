#include "search_handler.h"
#include <iostream>
#include <sstream>
#include <vector>

using json = nlohmann::json;

namespace search_handler {
    
    crow::response handle_search_posts(sqlite3* db, const crow::request& req) {
        // Get search query from URL parameters
        auto query_param = req.url_params.get("q");
        if (!query_param) {
            return crow::response(400, "Missing search query parameter");
        }
        
        std::string search_query = query_param;
        if (search_query.empty()) {
            json response;
            response["posts"] = json::array();
            return crow::response(200, response.dump());
        }
        
        // Prepare SQL query to search posts by content and username
        const char* sql = R"(
            SELECT p.id, p.user_id, p.content, p.created_at, u.username as user_name,
                   COUNT(DISTINCT l.id) as like_count,
                   COUNT(DISTINCT c.id) as comment_count
            FROM posts p
            JOIN users u ON p.user_id = u.id
            LEFT JOIN likes l ON p.id = l.post_id
            LEFT JOIN comments c ON p.id = c.post_id
            WHERE p.content LIKE ? OR u.username LIKE ?
            GROUP BY p.id, p.user_id, p.content, p.created_at, u.username
            ORDER BY p.created_at DESC
            LIMIT 20
        )";
        
        sqlite3_stmt* stmt;
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to prepare search statement: " << sqlite3_errmsg(db) << std::endl;
            return crow::response(500, "Database error");
        }
        
        // Bind search parameters with wildcards for LIKE search
        std::string search_pattern = "%" + search_query + "%";
        sqlite3_bind_text(stmt, 1, search_pattern.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, search_pattern.c_str(), -1, SQLITE_STATIC);
        
        json posts_array = json::array();
        
        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            json post;
            post["id"] = sqlite3_column_int(stmt, 0);
            post["user_id"] = sqlite3_column_int(stmt, 1);
            post["content"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            post["timestamp"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            post["user_name"] = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            post["like_count"] = sqlite3_column_int(stmt, 5);
            post["comment_count"] = sqlite3_column_int(stmt, 6);
            
            posts_array.push_back(post);
        }
        
        sqlite3_finalize(stmt);
        
        if (rc != SQLITE_DONE) {
            std::cerr << "Error executing search query: " << sqlite3_errmsg(db) << std::endl;
            return crow::response(500, "Database error");
        }
        
        json response;
        response["posts"] = posts_array;
        
        return crow::response(200, response.dump());
    }
}