#include "FriendManager.h"
#include "../database/db_utils.h"


void FriendManager::loadFriendsAVL(const std::string& username) {
    auto avl = std::make_unique<AVLTree>();

    std::string json = getFriendsList(db, username);

    std::vector<std::string> friends;
    if (json.size() > 2) { 
        size_t s = 1, e = json.size()-2;
        std::string curr;
        for (size_t i = s; i <= e; ++i) {
            if (json[i] == '\"') {
                size_t j = json.find('\"', i+1);
                if (j == std::string::npos) break;
                curr = json.substr(i+1, j-i-1);
                avl->insert(curr);
                i = j;
            }
        }
    }
    userFriends[username] = std::move(avl);
}

AVLTree* FriendManager::getAVL(const std::string& username) {
    if (userFriends.find(username) == userFriends.end()) {
        loadFriendsAVL(username);
    }
    return userFriends[username].get();
}

FriendManager::FriendManager(sqlite3* db_) : db(db_) {}


bool FriendManager::sendRequest(const std::string& from, const std::string& to) {
    if (from == to) return false; 
 
    if (areFriends(db, from, to) || hasPendingFriendRequest(db, from, to)) return false;
    return addFriend(db, from, to);
}

bool FriendManager::acceptRequest(const std::string& from, const std::string& to) {

    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    int fromId = -1, toId = -1;

    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, from.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) fromId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, to.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) toId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    if (fromId == -1 || toId == -1) return false;
    std::string query = "UPDATE friends SET status = 'accepted' WHERE requester_id = ? AND addressee_id = ? AND status = 'pending';";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, fromId);
    sqlite3_bind_int(stmt, 2, toId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    if (ok) {

        getAVL(from)->insert(to);
        getAVL(to)->insert(from);
    }
    return ok;
}

bool FriendManager::rejectRequest(const std::string& from, const std::string& to) {
  
    std::string userIdQuery = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    int fromId = -1, toId = -1;
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, from.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) fromId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    if (sqlite3_prepare_v2(db, userIdQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, to.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) toId = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
    }
    if (fromId == -1 || toId == -1) return false;
    std::string query = "DELETE FROM friends WHERE requester_id = ? AND addressee_id = ? AND status = 'pending';";
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, fromId);
    sqlite3_bind_int(stmt, 2, toId);
    bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return ok;
}

bool FriendManager::cancelRequest(const std::string& from, const std::string& to) {

    return rejectRequest(from, to);
}

bool FriendManager::removeFriend(const std::string& username, const std::string& friendUsername) {
    // Remove friendship from the database
    if (!removefriend(db, username, friendUsername)) return false;
    // Remove from both users' AVL trees
    getAVL(username)->remove(friendUsername);
    getAVL(friendUsername)->remove(username);
    return true;
}

std::vector<std::string> FriendManager::getFriends(const std::string& username) {
    AVLTree* tree = getAVL(username);
    return tree->inOrder();
}

void FriendManager::preloadUser(const std::string& username) {
    loadFriendsAVL(username);
}