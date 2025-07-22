#ifndef DYNAMIC_TIMELINE_SERVICE_H
#define DYNAMIC_TIMELINE_SERVICE_H

#include <vector>
#include <string>
#include <sqlite3.h>
#include <algorithm>
#include <unordered_set>
#include "Post.h"
#include "AVLtree.h"
#include "friend_suggestion_service.h"

/**
 * Dynamic Timeline Service
 * Generates timeline dynamically by:
 * 1. Loading friends from AVL tree
 * 2. Fetching posts from user and all friends
 * 3. Merging and sorting by timestamp efficiently
 * 4. Returning chronologically ordered timeline
 */
class DynamicTimelineService {
public:
    explicit DynamicTimelineService(sqlite3* db);
    
    /**
     * Generate dynamic timeline for a user
     * @param username The user to generate timeline for
     * @param limit Maximum number of posts to return (default: 50)
     * @return Vector of posts sorted by timestamp (newest first)
     */
    std::vector<Post> generateDynamicTimeline(const std::string& username, int limit = 50);

private:
    sqlite3* db;
    FriendSuggestionService friendService;
    
    /**
     * Load friends list using AVL tree from friend service
     * @param username User to get friends for
     * @return Vector of friend usernames
     */
    std::vector<std::string> loadFriendsFromAVL(const std::string& username);
    
    /**
     * Fetch posts for a specific user
     * @param username User to fetch posts for
     * @param limit Maximum posts to fetch
     * @return Vector of user's posts
     */
    std::vector<Post> fetchUserPosts(const std::string& username, int limit = 100);
    
    /**
     * Fetch posts from multiple friends efficiently
     * @param friendUsernames List of friend usernames
     * @param postsPerFriend Maximum posts per friend
     * @return Vector of all friends' posts
     */
    std::vector<Post> fetchFriendsPosts(const std::vector<std::string>& friendUsernames, int postsPerFriend = 20);
    
    /**
     * Merge user posts and friends posts efficiently
     * Uses merge sort algorithm for optimal performance
     * @param userPosts User's own posts
     * @param friendsPosts All friends' posts
     * @param limit Final limit for timeline
     * @return Merged and sorted timeline
     */
    std::vector<Post> mergePostsEfficiently(const std::vector<Post>& userPosts, 
                                          const std::vector<Post>& friendsPosts, 
                                          int limit);
    
    /**
     * Convert username to user_id for database queries
     * @param username Username to convert
     * @return User ID string
     */
    std::string getUserIdFromUsername(const std::string& username);
    
    /**
     * Get username from user_id for post display
     * @param userId User ID to convert
     * @return Username string
     */
    std::string getUsernameFromUserId(const std::string& userId);
    
    /**
     * Update post counts (likes, comments) from database
     * @param posts Posts to update counts for
     */
    void updatePostCounts(std::vector<Post>& posts);
};

#endif
