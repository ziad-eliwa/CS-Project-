#include "dynamic_timeline_service.h"
#include "../database/db_utils.h"
#include <iostream>
#include <nlohmann/json.hpp>

DynamicTimelineService::DynamicTimelineService(sqlite3* db) 
    : db(db), friendService(db) {}

std::vector<Post> DynamicTimelineService::generateDynamicTimeline(const std::string& username, int limit) {
    std::cout << "=== GENERATING DYNAMIC TIMELINE FOR: " << username << " ===" << std::endl;
    
    // Step 1: Load friends from AVL tree
    std::vector<std::string> friends = loadFriendsFromAVL(username);
    std::cout << "Loaded " << friends.size() << " friends from AVL tree" << std::endl;
    
    // Step 2: Fetch user's own posts
    std::vector<Post> userPosts = fetchUserPosts(username, 50);
    std::cout << "Fetched " << userPosts.size() << " user posts" << std::endl;
    
    // Step 3: Fetch friends' posts
    std::vector<Post> friendsPosts = fetchFriendsPosts(friends, 20);
    std::cout << "Fetched " << friendsPosts.size() << " friends posts" << std::endl;
    
    // Step 4: Merge posts efficiently using merge sort approach
    std::vector<Post> timeline = mergePostsEfficiently(userPosts, friendsPosts, limit);
    std::cout << "Generated timeline with " << timeline.size() << " posts" << std::endl;
    
    // Step 5: Update post counts (likes, comments)
    updatePostCounts(timeline);
    
    return timeline;
}

std::vector<std::string> DynamicTimelineService::loadFriendsFromAVL(const std::string& username) {
    // Use the existing friend service which uses AVL tree internally
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

std::vector<Post> DynamicTimelineService::fetchUserPosts(const std::string& username, int limit) {
    std::vector<Post> posts;
    std::string userId = getUserIdFromUsername(username);
    
    if (userId.empty()) {
        return posts;
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
            
            // Create post with all information (no image_url since it doesn't exist in schema)
            Post post(id, username, content, "", created_at, like_count, comment_count);
            posts.push_back(post);
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Error preparing user posts query: " << sqlite3_errmsg(db) << std::endl;
    }
    
    return posts;
}

std::vector<Post> DynamicTimelineService::fetchFriendsPosts(const std::vector<std::string>& friendUsernames, int postsPerFriend) {
    std::vector<Post> allFriendsPosts;
    
    for (const std::string& friendUsername : friendUsernames) {
        std::vector<Post> friendPosts = fetchUserPosts(friendUsername, postsPerFriend);
        allFriendsPosts.insert(allFriendsPosts.end(), friendPosts.begin(), friendPosts.end());
    }
    
    // Sort friends' posts by timestamp (newest first)
    std::sort(allFriendsPosts.begin(), allFriendsPosts.end(), 
              [](const Post& a, const Post& b) {
                  return a.getcreated_at() > b.getcreated_at();
              });
    
    return allFriendsPosts;
}

std::vector<Post> DynamicTimelineService::mergePostsEfficiently(const std::vector<Post>& userPosts, 
                                                               const std::vector<Post>& friendsPosts, 
                                                               int limit) {
    std::vector<Post> timeline;
    timeline.reserve(std::min(limit, static_cast<int>(userPosts.size() + friendsPosts.size())));
    
    // Use merge sort approach for efficient merging of two sorted arrays
    size_t i = 0, j = 0;
    
    while (i < userPosts.size() && j < friendsPosts.size() && timeline.size() < limit) {
        if (userPosts[i].getcreated_at() > friendsPosts[j].getcreated_at()) {
            timeline.push_back(userPosts[i]);
            i++;
        } else {
            timeline.push_back(friendsPosts[j]);
            j++;
        }
    }
    
    // Add remaining posts from user posts
    while (i < userPosts.size() && timeline.size() < limit) {
        timeline.push_back(userPosts[i]);
        i++;
    }
    
    // Add remaining posts from friends posts
    while (j < friendsPosts.size() && timeline.size() < limit) {
        timeline.push_back(friendsPosts[j]);
        j++;
    }
    
    return timeline;
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
    // Post counts are already fetched in the main query, but we can update them here if needed
    for (Post& post : posts) {
        // Counts are already set during fetchUserPosts, so this is mainly for future extensibility
        // We could add real-time count updates here if needed
    }
}
