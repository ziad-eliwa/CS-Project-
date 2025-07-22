#ifndef FRIEND_SUGGESTION_SERVICE_H
#define FRIEND_SUGGESTION_SERVICE_H

#pragma once

#include <vector>
#include <string>
#include <sqlite3.h>
#include "AVLtree.h"

// Service for friend suggestions and mutual friends
class FriendSuggestionService {
public:
    // Initializes with open DB pointer
    explicit FriendSuggestionService(sqlite3* db);

    // Get mutual friends for two usernames
    std::vector<std::string> getMutualFriends(const std::string& user1, const std::string& user2);

    // Suggest friends for 'username' based on 2nd-degree connections (not direct friends, but friends of friends)
    // Returns suggestion pairs: (username, #mutual_friends)
    std::vector<std::pair<std::string, int>> suggestFriends(const std::string& username, int maxResults = 10);

private:
    sqlite3* db;

    // Loads an AVLTree of friends for the given username (direct friends only, by username)
    AVLTree loadFriendsAVL(const std::string& username);

    // Helper: returns all users except 'exclude' and direct friends as a vector
    std::vector<std::string> getNonFriendUsers(const std::string& username, const std::vector<std::string>& directFriends, int maxResults);

};




#endif