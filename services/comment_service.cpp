#include "comment_service.h"
#include <iostream>
#include <string>

#define DB_PATH "../database/users.db"

static int createComment(sqlite3* db, int postId, const std::string& username, const std::string& comment){
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
    std::string query = "INSERT INTO comments (post_id, user_id, content) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return -1;
    sqlite3_bind_int(stmt, 1, postId);
    sqlite3_bind_int(stmt, 2, userId);
    sqlite3_bind_text(stmt, 3, comment.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    if (success) {
        std::string updateQuery = "UPDATE posts SET comment_count = comment_count + 1 WHERE id = ?;";
        sqlite3_stmt* updateStmt;
        if (sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &updateStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(updateStmt, 1, postId);
            sqlite3_step(updateStmt);
            sqlite3_finalize(updateStmt);
        }
        return sqlite3_last_insert_rowid(db);
    }
    return -1;
}

static bool deleteComment(sqlite3* db, int commentId){
    int postId = -1;
    std::string postIdQuery = "SELECT post_id FROM comments WHERE id = ?;";
    sqlite3_stmt* postStmt;
    if (sqlite3_prepare_v2(db, postIdQuery.c_str(), -1, &postStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(postStmt, 1, commentId);
        if (sqlite3_step(postStmt) == SQLITE_ROW) {
            postId = sqlite3_column_int(postStmt, 0);
        }
        sqlite3_finalize(postStmt);
    }
    if (postId == -1) return false;
    std::string query = "DELETE FROM comments WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, commentId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    if (success) {
        std::string updateQuery = "UPDATE posts SET comment_count = comment_count - 1 WHERE id = ? AND comment_count > 0;";
        sqlite3_stmt* updateStmt;
        if (sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &updateStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(updateStmt, 1, postId);
            sqlite3_step(updateStmt);
            sqlite3_finalize(updateStmt);
        }
    }
    return success;
}


std::vector<Comment> CommentService::getCommentsByPost(int post_id) {
    sqlite3* db;
    sqlite3_open(DB_PATH, &db);
    std::vector<Comment> comments;
    std::string query = "SELECT comments.id, comments.post_id, users.username, comments.content, comments.created_at "
                        "FROM comments JOIN users ON comments.user_id = users.id "
                        "WHERE comments.post_id = ? ORDER BY comments.created_at ASC";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, post_id);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Comment comment;
            comment.setId(sqlite3_column_int(stmt, 0));
            comment.setPostId(sqlite3_column_int(stmt, 1));
            comment.setUserName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            comment.setContent(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            comment.setCreatedAt(static_cast<std::time_t>(sqlite3_column_int64(stmt, 4)));
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
            comment->setId(sqlite3_column_int(stmt, 0));
            comment->setPostId(sqlite3_column_int(stmt, 1));
            comment->setUserName(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            comment->setContent(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            comment->setCreatedAt(static_cast<std::time_t>(sqlite3_column_int64(stmt, 4)));
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

int CommentService::createComment(sqlite3* db, int postId, const std::string& username, const std::string& comment) {
    return ::createComment(db, postId, username, comment);
}

bool CommentService::deleteComment(sqlite3* db, int commentId) {
    return ::deleteComment(db, commentId);
}


