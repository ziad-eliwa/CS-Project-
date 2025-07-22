#ifndef FRIEND_MANAGER_H
#define FRIEND_MANAGER_H

#include <string>
#include <unordered_map>
#include <memory>
#include "AVLtree.h"
#include <sqlite3.h>


class FriendManager {
private:
    sqlite3* db;

    std::unordered_map<std::string, std::unique_ptr<AVLTree>> userFriends;

    void loadFriendsAVL(const std::string& username);


    AVLTree* getAVL(const std::string& username);

public:
    explicit FriendManager(sqlite3* db);


    bool sendRequest(const std::string& from, const std::string& to);


    bool acceptRequest(const std::string& from, const std::string& to);


    bool rejectRequest(const std::string& from, const std::string& to);


    bool cancelRequest(const std::string& from, const std::string& to);


    bool removeFriend(const std::string& username, const std::string& friendUsername);


    std::vector<std::string> getFriends(const std::string& username);


    void preloadUser(const std::string& username);
};

#endif