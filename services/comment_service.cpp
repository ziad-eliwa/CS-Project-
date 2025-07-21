#include "comment_service.h"
#include <iostream>

#define DB_PATH "../database/users.db"

std::vector<Comment> CommentService::getCommentsByPost(int post_id) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);
    std::vector<Comment> comments;
    std::string query = "SELECT id, post_id, user_name, content, created_at FROM comments WHERE post_id = ? ORDER BY created_at ASC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, post_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Comment comment;
            comment.id = sqlite3_column_int(stmt, 0);
            comment.post_id = sqlite3_column_int(stmt, 1);
            comment.user_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            comment.content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            comment.created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 4));
            comments.push_back(comment);
        }
    } else {
        std::cerr << "Select failed: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return comments;
}

Comment* CommentService::getCommentById(int comment_id) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);
    std::string query = "SELECT id, post_id, user_name, content, created_at FROM comments WHERE id = ?";
    sqlite3_stmt* stmt;
    Comment* comment = nullptr;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, comment_id);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            comment = new Comment();
            comment->id = sqlite3_column_int(stmt, 0);
            comment->post_id = sqlite3_column_int(stmt, 1);
            comment->user_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            comment->content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            comment->created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 4));
        }
    } else {
        std::cerr << "Select failed: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return comment;
}

bool CommentService::updateComment(int comment_id, const std::string& new_content) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);
    std::string query = "UPDATE comments SET content = ? WHERE id = ?";
    sqlite3_stmt* stmt;
    bool success = false;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, new_content.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, comment_id);
        success = (sqlite3_step(stmt) == SQLITE_DONE);
        if (!success) {
            std::cerr << "Update comment failed: " << sqlite3_errmsg(db) << std::endl;
        }
    } else {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return success;
}


