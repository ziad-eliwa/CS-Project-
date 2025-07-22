#include "like_service.h"
#include <sqlite3.h>
#include <iostream>

bool LikeService::toggleLike(sqlite3* db, int post_id, const std::string& username) {
    // First, get the user_id from username
    const char* get_user_sql = "SELECT id FROM users WHERE username = ?";
    sqlite3_stmt* stmt;
    int user_id = -1;
    
    if (sqlite3_prepare_v2(db, get_user_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            user_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    
    if (user_id == -1) {
        std::cerr << "User not found: " << username << std::endl;
        return false;
    }
    
    // Check if user already liked this post
    const char* check_like_sql = "SELECT COUNT(*) FROM likes WHERE user_id = ? AND post_id = ?";
    bool already_liked = false;
    
    if (sqlite3_prepare_v2(db, check_like_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, user_id);
        sqlite3_bind_int(stmt, 2, post_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            already_liked = sqlite3_column_int(stmt, 0) > 0;
        }
        sqlite3_finalize(stmt);
    }
    
    if (already_liked) {
        // Unlike: Remove like and decrement count
        const char* delete_like_sql = "DELETE FROM likes WHERE user_id = ? AND post_id = ?";
        if (sqlite3_prepare_v2(db, delete_like_sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, user_id);
            sqlite3_bind_int(stmt, 2, post_id);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        
        const char* decrement_sql = "UPDATE posts SET like_count = like_count - 1 WHERE id = ?";
        if (sqlite3_prepare_v2(db, decrement_sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, post_id);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        
        return true; // Successfully unliked
    } else {
        // Like: Add like and increment count
        const char* insert_like_sql = "INSERT INTO likes (user_id, post_id) VALUES (?, ?)";
        if (sqlite3_prepare_v2(db, insert_like_sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, user_id);
            sqlite3_bind_int(stmt, 2, post_id);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        
        const char* increment_sql = "UPDATE posts SET like_count = like_count + 1 WHERE id = ?";
        if (sqlite3_prepare_v2(db, increment_sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, post_id);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
        
        return true; // Successfully liked
    }
}

bool LikeService::isPostLikedByUser(sqlite3* db, int post_id, const std::string& username) {
    // Get user_id from username
    const char* get_user_sql = "SELECT id FROM users WHERE username = ?";
    sqlite3_stmt* stmt;
    int user_id = -1;
    
    if (sqlite3_prepare_v2(db, get_user_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            user_id = sqlite3_column_int(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }
    
    if (user_id == -1) return false;
    
    // Check if like exists
    const char* check_like_sql = "SELECT COUNT(*) FROM likes WHERE user_id = ? AND post_id = ?";
    bool is_liked = false;
    
    if (sqlite3_prepare_v2(db, check_like_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, user_id);
        sqlite3_bind_int(stmt, 2, post_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            is_liked = sqlite3_column_int(stmt, 0) > 0;
        }
        sqlite3_finalize(stmt);
    }
    
    return is_liked;
}
