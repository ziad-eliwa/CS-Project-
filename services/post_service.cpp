// post_service.cpp
#include "post_service.h"
#include <iostream>

// Replace this with your actual database path
#define DB_PATH "path_to_your_database.db"

bool PostService::createPost(const std::string& user_name, const std::string& content,
                             const std::string& image_url, const std::string& privacy) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);  // open the DB

    std::string query = "INSERT INTO posts (user_name, content, image_url, timestamp, privacy) "
                        "VALUES (?, ?, ?, datetime('now'), ?)";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }

    // Bind values to the query
    sqlite3_bind_text(stmt, 1, user_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, image_url.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, privacy.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    if (!success) {
        std::cerr << "Insert failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}

std::vector<Post> PostService::getAllPosts() {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);

    std::vector<Post> posts;
    std::string query = "SELECT id, user_name, content, image_url, timestamp, privacy FROM posts ORDER BY timestamp DESC";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Post post;
            post.id = sqlite3_column_int(stmt, 0);
            post.user_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            post.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            post.image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            post.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            post.privacy = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            posts.push_back(post);
        }
    } else {
        std::cerr << "Select failed: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return posts;
}
