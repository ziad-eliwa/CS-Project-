#include "friend_suggestion_service.h"
#include "../database/db_utils.h"   // for getFriendsList, getAllUsersExcept, areFriends
#include <nlohmann/json.hpp>
#include <unordered_set>
#include <algorithm>

FriendSuggestionService::FriendSuggestionService(sqlite3* db_) : db(db_) {}

AVLTree FriendSuggestionService::loadFriendsAVL(const std::string& username) {
    AVLTree tree;
    std::string jsonList = getFriendsList(db, username);
    nlohmann::json j = nlohmann::json::parse(jsonList);
    for (auto& friendname : j) {
        tree.insert(friendname);
    }
    return tree;
}

std::vector<std::string> FriendSuggestionService::getMutualFriends(const std::string& user1, const std::string& user2) {
    AVLTree friends1 = loadFriendsAVL(user1);
    AVLTree friends2 = loadFriendsAVL(user2);
    std::vector<std::string> f1 = friends1.inOrder();
    std::vector<std::string> f2 = friends2.inOrder();

    std::vector<std::string> mutual;
    std::set_intersection(f1.begin(), f1.end(), f2.begin(), f2.end(), std::back_inserter(mutual));
    return mutual;
}

std::vector<std::string> FriendSuggestionService::getNonFriendUsers(
    const std::string& username,
    const std::vector<std::string>& directFriends,
    int maxResults
) {
    // Reuse DB utility if it returns only users who are not yourself or your direct friends
    std::vector<std::string> all = getAllUsersExcept(db, username, 500); // Large enough for filtering

    std::unordered_set<std::string> friendsSet(directFriends.begin(), directFriends.end());
    friendsSet.insert(username);

    std::vector<std::string> candidates;
    for (const auto& user : all) {
        if (friendsSet.count(user) == 0) candidates.push_back(user);
        if ((int)candidates.size() >= maxResults) break;
    }
    return candidates;
}

std::vector<std::pair<std::string, int>> FriendSuggestionService::suggestFriends(const std::string& username, int maxResults) {
    // Direct friends
    AVLTree userFriendsAVL = loadFriendsAVL(username);
    std::vector<std::string> directFriends = userFriendsAVL.inOrder();

    // Set of all direct friends for fast lookup
    std::unordered_set<std::string> directSet(directFriends.begin(), directFriends.end());

    // Candidates: users who are NOT direct friends and NOT yourself
    std::vector<std::string> candidates = getNonFriendUsers(username, directFriends, 1000);

    // Count mutuals for each candidate
    std::vector<std::pair<std::string, int>> suggestions;

    for (const auto& candidate : candidates) {
        AVLTree candidateAVL = loadFriendsAVL(candidate);
        std::vector<std::string> theirFriends = candidateAVL.inOrder();
        int mutualCount = 0;
        // Efficient intersection count: both sorted
        auto it1 = directFriends.begin(), it2 = theirFriends.begin();
        while (it1 != directFriends.end() && it2 != theirFriends.end()) {
            if (*it1 == *it2) { ++mutualCount; ++it1; ++it2; }
            else if (*it1 < *it2) { ++it1; }
            else { ++it2; }
        }
        // Include all candidates, but prioritize those with mutual friends
        suggestions.emplace_back(candidate, mutualCount);
    }

    // Sort descending by mutualCount, then lexically by username
    std::sort(suggestions.begin(), suggestions.end(), [](auto& a, auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });

    // Trim to desired maxResults
    if ((int)suggestions.size() > maxResults)
        suggestions.resize(maxResults);

    return suggestions;
}