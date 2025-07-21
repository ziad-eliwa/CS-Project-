
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

std::vector<Post> getPostsByUser(sqlite3* db, const std::string& username) {
    std::vector<Post> posts;

    std::string query = "SELECT posts.id, posts.content, posts.like_count, posts.comment_count, posts.created_at "
                        "FROM posts JOIN users ON posts.user_id = users.id "
                        "WHERE users.username = ? ORDER BY posts.created_at DESC;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Post post;
            post.id = sqlite3_column_int(stmt, 0);
            post.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            post.like_count = sqlite3_column_int(stmt, 2);
            post.comment_count = sqlite3_column_int(stmt, 3);
            post.created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

            posts.push_back(post);
        }
        sqlite3_finalize(stmt);
    }

    return posts;
}

static bool deletePost(sqlite3* db, int post_id){
    std::string query = "DELETE FROM posts WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, postId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

static int createPost(sqlite3* db, const std::string& username, const std::string& content, const std::string& image_url){
    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* userStmt;
    int userId = -1;
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) {
            userId = sqlite3_column_int(userStmt, 0);
        }
        sqlite3_finalize(userStmt);
    }
    if (userId == -1) return -1;
    std::string query = "INSERT INTO posts (user_id, content) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_text(stmt, 2, content.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    if (success) {
        return sqlite3_last_insert_rowid(db);
    }
    return -1;
}
