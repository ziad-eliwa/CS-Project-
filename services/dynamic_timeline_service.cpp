#include "dynamic_timeline_service.h"
#include "../database/db_utils.h"
#include <iostream>
#include <nlohmann/json.hpp>

DynamicTimelineService::DynamicTimelineService(sqlite3* db) 
    : db(db), friendService(db) {}

std::vector<Post> DynamicTimelineService::generateDynamicTimeline(const std::string& username, int limit) {
    std::cout << "=== GENERATING AVL-MANAGED DYNAMIC TIMELINE FOR: " << username << " ===" << std::endl;
    
    timelineTree.clear();
    
    loadAllPostsIntoAVL(username);
    
    std::vector<Post> timeline = timelineTree.getTimelineInOrder(limit);
    std::cout << "Generated AVL timeline with " << timeline.size() << " posts" << std::endl;
    
    updatePostCounts(timeline);
    
    return timeline;
}

std::vector<std::string> DynamicTimelineService::loadFriendsFromAVL(const std::string& username) {
    std::string jsonFriends = getFriendsList(db, username);
    
    std::vector<std::string> friends;
    try {
        nlohmann::json j = nlohmann::json::parse(jsonFriends);
        for (const auto& friendName : j) {
            friends.push_back(friendName.get<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing friends JSON: " << e.what() << std::endl;
    }
    return friends;
}

void DynamicTimelineService::addPostToTimeline(const Post& post) {
    timelineTree.insert(post);
}

void DynamicTimelineService::removePostFromTimeline(int postId) {
    timelineTree.remove(postId);
}

void DynamicTimelineService::clearTimeline() {
    timelineTree.clear();
}

void DynamicTimelineService::loadAllPostsIntoAVL(const std::string& username) {
    std::cout << "Loading posts into AVL tree for user: " << username << std::endl;
    loadUserPostsIntoAVL(username, 50);
    std::vector<std::string> friends = loadFriendsFromAVL(username);
    std::cout << "Loaded " << friends.size() << " friends from AVL tree" << std::endl;
    loadFriendsPostsIntoAVL(friends, 20);
    std::cout << "Total posts in AVL tree: " << timelineTree.size() << std::endl;
}

void DynamicTimelineService::loadUserPostsIntoAVL(const std::string& username, int limit) {
    std::string userId = getUserIdFromUsername(username);
    
    if (userId.empty()) {
        return;
    }
    
    std::string query = R"(
        SELECT p.id, p.content, p.user_id, p.created_at,
               COALESCE(l.like_count, 0) as like_count,
               COALESCE(c.comment_count, 0) as comment_count
        FROM posts p
        LEFT JOIN (
            SELECT post_id, COUNT(*) as like_count 
            FROM likes 
            GROUP BY post_id
        ) l ON p.id = l.post_id
        LEFT JOIN (
            SELECT post_id, COUNT(*) as comment_count 
            FROM comments 
            GROUP BY post_id
        ) c ON p.id = c.post_id
        WHERE p.user_id = ?
        ORDER BY p.created_at DESC
        LIMIT ?
    )";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, limit);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string user_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::time_t created_at = static_cast<std::time_t>(sqlite3_column_int64(stmt, 3));
            int like_count = sqlite3_column_int(stmt, 4);
            int comment_count = sqlite3_column_int(stmt, 5);
            
            Post post(id, username, content, "", created_at, like_count, comment_count);
            timelineTree.insert(post);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Error preparing user posts query: " << sqlite3_errmsg(db) << std::endl;
    }
}

void DynamicTimelineService::loadFriendsPostsIntoAVL(const std::vector<std::string>& friendUsernames, int postsPerFriend) {
    for (const std::string& friendUsername : friendUsernames) {
        loadUserPostsIntoAVL(friendUsername, postsPerFriend);
    }
}

std::string DynamicTimelineService::getUserIdFromUsername(const std::string& username) {
    std::string userId;
    std::string query = "SELECT id FROM users WHERE username = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            userId = std::to_string(sqlite3_column_int(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    
    return userId;
}

std::string DynamicTimelineService::getUsernameFromUserId(const std::string& userId) {
    std::string username;
    std::string query = "SELECT username FROM users WHERE id = ?";
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, userId.c_str(), -1, SQLITE_STATIC);
        
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    
    return username;
}

void DynamicTimelineService::updatePostCounts(std::vector<Post>& posts) {
    for (Post& post : posts) {

    }
}
