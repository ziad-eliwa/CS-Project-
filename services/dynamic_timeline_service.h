#ifndef DYNAMIC_TIMELINE_SERVICE_H
#define DYNAMIC_TIMELINE_SERVICE_H

#include <vector>
#include <string>
#include <sqlite3.h>
#include <algorithm>
#include <unordered_set>
#include "Post.h"
#include "PostAVLTree.h"
#include "friend_suggestion_service.h"

class DynamicTimelineService {
public:
    explicit DynamicTimelineService(sqlite3* db);
    std::vector<Post> generateDynamicTimeline(const std::string& username, int limit = 50);
    void addPostToTimeline(const Post& post);
    void removePostFromTimeline(int postId);
    void clearTimeline();

private:
    sqlite3* db;
    FriendSuggestionService friendService;
    PostAVLTree timelineTree;  
    
    std::vector<std::string> loadFriendsFromAVL(const std::string& username);
    void loadAllPostsIntoAVL(const std::string& username);
    void loadUserPostsIntoAVL(const std::string& username, int limit = 100);
    void loadFriendsPostsIntoAVL(const std::vector<std::string>& friendUsernames, int postsPerFriend = 20);
    std::string getUserIdFromUsername(const std::string& username);
    std::string getUsernameFromUserId(const std::string& userId);
    void updatePostCounts(std::vector<Post>& posts);
};

#endif
