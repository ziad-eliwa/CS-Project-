#include "db_utils.h"
#include "../utils/hash_utils.h"
#include <vector>
#include <sstream>

bool checkCredentials(sqlite3* db, const std::string& username, const std::string& password){
    std::string hashedPassword = hashed(password);
    std::string query = "SELECT * FROM users WHERE username = ? AND password_hash = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);

    bool found = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return found;
}

bool userExists(sqlite3* db, const std::string& username){
    std::string query = "SELECT 1 FROM users WHERE username = ?;";
    
    sqlite3_stmt * stmt;

    sqlite3_prepare_v2(db,query.c_str(),-1,&stmt,nullptr);

    sqlite3_bind_text(stmt,1,username.c_str(),-1,SQLITE_STATIC);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;    
}

bool registerUser(sqlite3* db, const std::string& username, const std::string& password){
    std::string hashedPassword = hashed(password);
    std::string query = "INSERT INTO users (username,password_hash) VALUES(?,?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
// Posts
int createPost(sqlite3* db, const std::string& username, const std::string& content) {
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

bool deletePost(sqlite3* db, int postId) {
    std::string query = "DELETE FROM posts WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, postId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
// Feed database utilities
std::string getFeed(sqlite3* db, const std::string& username) {
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
    if (userId == -1) return "[]";
    // Get friend ids
    std::vector<int> friendIds;
    std::string friendsQuery = "SELECT addressee_id FROM friends WHERE requester_id = ? AND status = 'accepted' UNION SELECT requester_id FROM friends WHERE addressee_id = ? AND status = 'accepted';";
    sqlite3_stmt* friendsStmt;
    if (sqlite3_prepare_v2(db, friendsQuery.c_str(), -1, &friendsStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(friendsStmt, 1, userId);
        sqlite3_bind_int(friendsStmt, 2, userId);
        while (sqlite3_step(friendsStmt) == SQLITE_ROW) {
            friendIds.push_back(sqlite3_column_int(friendsStmt, 0));
        }
        sqlite3_finalize(friendsStmt);
    }
    friendIds.push_back(userId); // Include own posts
    // Build query for posts
    std::ostringstream oss;
    oss << "SELECT posts.id, users.username, posts.content, posts.like_count, posts.comment_count, posts.created_at FROM posts JOIN users ON posts.user_id = users.id WHERE posts.user_id IN (";
    for (size_t i = 0; i < friendIds.size(); ++i) {
        oss << friendIds[i];
        if (i < friendIds.size() - 1) oss << ",";
    }
    oss << ") ORDER BY posts.created_at DESC;";
    std::string postsQuery = oss.str();
    
    sqlite3_stmt* postsStmt;
    std::string feedData = "[";
    bool first = true;
    if (sqlite3_prepare_v2(db, postsQuery.c_str(), -1, &postsStmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(postsStmt) == SQLITE_ROW) {
            if (!first) feedData += ",";
            first = false;
            feedData += "{\"post_id\":" + std::to_string(sqlite3_column_int(postsStmt, 0)) +
                        ",\"username\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(postsStmt, 1))) +
                        "\",\"content\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(postsStmt, 2))) +
                        "\",\"like_count\":" + std::to_string(sqlite3_column_int(postsStmt, 3)) +
                        ",\"comment_count\":" + std::to_string(sqlite3_column_int(postsStmt, 4)) +
                        ",\"created_at\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(postsStmt, 5))) + "\"}";
        }
        sqlite3_finalize(postsStmt);
    }
    feedData += "]";
    return feedData;
}

std::string getUserPosts(sqlite3* db, const std::string& username) {
    std::string query = "SELECT posts.id, posts.content, posts.like_count, posts.comment_count, posts.created_at FROM posts JOIN users ON posts.user_id = users.id WHERE users.username = ? ORDER BY posts.created_at DESC;";
    sqlite3_stmt* stmt;
    std::string userPostsData = "[";
    bool first = true;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) userPostsData += ",";
            first = false;
            userPostsData += "{\"post_id\":" + std::to_string(sqlite3_column_int(stmt, 0)) +
                             ",\"content\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) +
                             "\",\"like_count\":" + std::to_string(sqlite3_column_int(stmt, 2)) +
                             ",\"comment_count\":" + std::to_string(sqlite3_column_int(stmt, 3)) +
                             ",\"created_at\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))) + "\"}";
        }
        sqlite3_finalize(stmt);
    }
    userPostsData += "]";
    return userPostsData;
}

bool likePost(sqlite3* db, int postId, const std::string& username) {
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
    if (userId == -1) return false;
    std::string query = "INSERT INTO likes (user_id, post_id) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, postId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    if (success) {
        std::string updateQuery = "UPDATE posts SET like_count = like_count + 1 WHERE id = ?;";
        sqlite3_stmt* updateStmt;
        if (sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &updateStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(updateStmt, 1, postId);
            sqlite3_step(updateStmt);
            sqlite3_finalize(updateStmt);
        }
    }
    return success;
}

bool unlikePost(sqlite3* db, int postId, const std::string& username) {
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
    if (userId == -1) return false;
    std::string query = "DELETE FROM likes WHERE user_id = ? AND post_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, postId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    if (success) {
        std::string updateQuery = "UPDATE posts SET like_count = like_count - 1 WHERE id = ? AND like_count > 0;";
        sqlite3_stmt* updateStmt;
        if (sqlite3_prepare_v2(db, updateQuery.c_str(), -1, &updateStmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(updateStmt, 1, postId);
            sqlite3_step(updateStmt);
            sqlite3_finalize(updateStmt);
        }
    }
    return success;
}

int commentOnPost(sqlite3* db, int postId, const std::string& username, const std::string& comment) {
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

bool deleteComment(sqlite3* db, int commentId) {
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
// friends
bool addFriend(sqlite3* db, const std::string& username, const std::string& friendUsername) {
    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* userStmt;
    int userId = -1, friendId = -1;
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) userId = sqlite3_column_int(userStmt, 0);
        sqlite3_finalize(userStmt);
    }
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, friendUsername.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) friendId = sqlite3_column_int(userStmt, 0);
        sqlite3_finalize(userStmt);
    }
    if (userId == -1 || friendId == -1) return false;
    std::string query = "INSERT INTO friends (requester_id, addressee_id, status) VALUES (?, ?, 'pending');";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, friendId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool removefriend(sqlite3* db, const std::string& username, const std::string& friendUsername) {
    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* userStmt;
    int userId = -1, friendId = -1;
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) userId = sqlite3_column_int(userStmt, 0);
        sqlite3_finalize(userStmt);
    }
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, friendUsername.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) friendId = sqlite3_column_int(userStmt, 0);
        sqlite3_finalize(userStmt);
    }
    if (userId == -1 || friendId == -1) return false;
    std::string query = "DELETE FROM friends WHERE (requester_id = ? AND addressee_id = ?) OR (requester_id = ? AND addressee_id = ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, userId);
    sqlite3_bind_int(stmt, 2, friendId);
    sqlite3_bind_int(stmt, 3, friendId);
    sqlite3_bind_int(stmt, 4, userId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

std::string getFriendsList(sqlite3* db, const std::string& username) {
    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* userStmt;
    int userId = -1;
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) userId = sqlite3_column_int(userStmt, 0);
        sqlite3_finalize(userStmt);
    }
    if (userId == -1) return "[]";
    std::string query = "SELECT u.username FROM users u JOIN friends f ON (u.id = f.addressee_id OR u.id = f.requester_id) WHERE f.status = 'accepted' AND (f.requester_id = ? OR f.addressee_id = ?) AND u.id != ?;";
    sqlite3_stmt* stmt;
    std::string friendsListData = "[";
    bool first = true;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_int(stmt, 2, userId);
        sqlite3_bind_int(stmt, 3, userId);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) friendsListData += ",";
            first = false;
            friendsListData += "\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) + "\"";
        }
        sqlite3_finalize(stmt);
    }
    friendsListData += "]";
    return friendsListData;
}

std::string getProfile(sqlite3* db, const std::string& username) {
    std::string query = "SELECT display_name, avatar_url, bio, location, website FROM profile JOIN users ON profile.user_id = users.id WHERE users.username = ?;";
    sqlite3_stmt* stmt;
    std::string profileData = "{";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            profileData += "\"display_name\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))) +
                           "\",\"avatar_url\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) +
                           "\",\"bio\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))) +
                           "\",\"location\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3))) +
                           "\",\"website\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))) + "\"";
        }
        sqlite3_finalize(stmt);
    }
    profileData += "}";
    return profileData;
}

bool updateProfile(sqlite3* db, const std::string& username, const std::string& newProfileData) {
    // newProfileData should be a JSON string, parse it accordingly (pseudo-code)
    // For simplicity, assume newProfileData is display_name only
    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* userStmt;
    int userId = -1;
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) userId = sqlite3_column_int(userStmt, 0);
        sqlite3_finalize(userStmt);
    }
    if (userId == -1) return false;
    std::string query = "UPDATE profile SET display_name = ? WHERE user_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, newProfileData.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, userId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
std::string getNotifications(sqlite3* db, const std::string& username) {
    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* userStmt;
    int userId = -1;
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &userStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(userStmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(userStmt) == SQLITE_ROW) userId = sqlite3_column_int(userStmt, 0);
        sqlite3_finalize(userStmt);
    }
    if (userId == -1) return "[]";
    std::string query = "SELECT id, type, message, is_read, created_at FROM notifications WHERE user_id = ? ORDER BY created_at DESC;";
    sqlite3_stmt* stmt;
    std::string notificationsData = "[";
    bool first = true;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            if (!first) notificationsData += ",";
            first = false;
            notificationsData += "{\"id\":" + std::to_string(sqlite3_column_int(stmt, 0)) +
                                 ",\"type\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) +
                                 "\",\"message\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2))) +
                                 "\",\"is_read\":" + std::to_string(sqlite3_column_int(stmt, 3)) +
                                 ",\"created_at\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4))) + "\"}";
        }
        sqlite3_finalize(stmt);
    }
    notificationsData += "]";
    return notificationsData;
}

bool markNotificationAsRead(sqlite3* db, int notificationId) {
    std::string query = "UPDATE notifications SET is_read = 1 WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, notificationId);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
// Friend search helper functions
std::vector<std::string> searchUsersByPrefix(sqlite3* db, const std::string& prefix, int limit) {
    std::vector<std::string> users;
    std::string query = "SELECT username FROM users WHERE username LIKE ? ORDER BY username LIMIT ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string searchPattern = prefix + "%";
        sqlite3_bind_text(stmt, 1, searchPattern.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, limit);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            users.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
        }
        sqlite3_finalize(stmt);
    }
    return users;
}

std::vector<std::string> getAllUsersExcept(sqlite3* db, const std::string& excludeUsername, int limit) {
    std::vector<std::string> users;
    std::string query = "SELECT username FROM users WHERE username != ? ORDER BY username LIMIT ?;";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, excludeUsername.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, limit);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            users.push_back(std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))));
        }
        sqlite3_finalize(stmt);
    }
    return users;
}

bool areFriends(sqlite3* db, const std::string& user1, const std::string& user2) {
    std::string query = R"(
        SELECT COUNT(*) FROM friends f
        JOIN users u1 ON f.requester_id = u1.id
        JOIN users u2 ON f.addressee_id = u2.id
        WHERE f.status = 'accepted' AND (
            (u1.username = ? AND u2.username = ?) OR
            (u1.username = ? AND u2.username = ?)
        )
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user1.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, user2.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, user2.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, user1.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            bool friends = sqlite3_column_int(stmt, 0) > 0;
            sqlite3_finalize(stmt);
            return friends;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

bool hasPendingFriendRequest(sqlite3* db, const std::string& requester, const std::string& addressee) {
    std::string query = R"(
        SELECT COUNT(*) FROM friends f
        JOIN users u1 ON f.requester_id = u1.id
        JOIN users u2 ON f.addressee_id = u2.id
        WHERE f.status = 'pending' AND (
            (u1.username = ? AND u2.username = ?) OR
            (u1.username = ? AND u2.username = ?)
        )
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, addressee.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, addressee.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, requester.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            bool pending = sqlite3_column_int(stmt, 0) > 0;
            sqlite3_finalize(stmt);
            return pending;
        }
        sqlite3_finalize(stmt);
    }
    return false;
}

std::string getUserInfo(sqlite3* db, const std::string& username) {
    std::string query = "SELECT id, username, email, profile_pic, bio, created_at FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    std::string userInfo = "{}";
    
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            userInfo = "{";
            userInfo += "\"id\":" + std::to_string(sqlite3_column_int(stmt, 0)) + ",";
            userInfo += "\"username\":\"" + std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1))) + "\",";
            
            const char* email = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            userInfo += "\"email\":\"" + std::string(email ? email : "") + "\",";
            
            const char* profile_pic = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            userInfo += "\"profile_pic\":\"" + std::string(profile_pic ? profile_pic : "") + "\",";
            
            const char* bio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            userInfo += "\"bio\":\"" + std::string(bio ? bio : "") + "\",";
            
            const char* created_at = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
            userInfo += "\"created_at\":\"" + std::string(created_at ? created_at : "") + "\"";
            
            userInfo += "}";
        }
        sqlite3_finalize(stmt);
    }
    return userInfo;
}