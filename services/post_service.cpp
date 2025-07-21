
// post_service.cpp
#include "post_service.h"
#include <iostream>

// Replace this with your actual database path
#define DB_PATH "../database/users.db"


Post* PostService::getPostById(int post_id) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);
    
    std::string query = "SELECT id, user_name, content, image_url, created_at, like_count, comment_count FROM posts WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return nullptr;
    }
    
    sqlite3_bind_int(stmt, 1, post_id);
    
    Post* post = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string user_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string image_url = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::time_t created_at = sqlite3_column_int64(stmt, 4);
        int like_count = sqlite3_column_int(stmt, 5);
        int comment_count = sqlite3_column_int(stmt, 6);
        
        post = new Post(id, user_name, content, image_url, created_at, like_count, comment_count);
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return post;
}

bool PostService::updatePost(int post_id, const std::string& new_content,
                             const std::string& new_image_url, const std::string& new_privacy) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);
    
    std::string query = "UPDATE posts SET content = ?, image_url = ?, privacy = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, new_content.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, new_image_url.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, new_privacy.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, post_id);
    
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    
    return success;
}
