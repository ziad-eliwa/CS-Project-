#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <crow.h>

// User data structure for search results
struct SearchUser {
    int id;
    std::string username;
    std::string email;
    std::string profile_pic;
    std::string bio;
    std::string created_at;
    bool is_friend;
    bool has_pending_request;
    
    SearchUser() : id(0), is_friend(false), has_pending_request(false) {}
    
    SearchUser(int _id, const std::string& _username, const std::string& _email,
               const std::string& _profile_pic, const std::string& _bio, 
               const std::string& _created_at)
        : id(_id), username(_username), email(_email), profile_pic(_profile_pic),
          bio(_bio), created_at(_created_at), is_friend(false), has_pending_request(false) {}
};

// BST Node for efficient prefix searching
struct BSTNode {
    SearchUser user;
    std::shared_ptr<BSTNode> left;
    std::shared_ptr<BSTNode> right;
    
    BSTNode(const SearchUser& _user) : user(_user), left(nullptr), right(nullptr) {}
};

// Binary Search Tree for user search
class UserSearchBST {
private:
    std::shared_ptr<BSTNode> root;
    
    // Helper functions
    std::shared_ptr<BSTNode> insertNode(std::shared_ptr<BSTNode> node, const SearchUser& user);
    void inorderTraversal(std::shared_ptr<BSTNode> node, std::vector<SearchUser>& result);
    void prefixSearch(std::shared_ptr<BSTNode> node, const std::string& prefix, 
                     std::vector<SearchUser>& result);
    void clearTree(std::shared_ptr<BSTNode> node);
    
public:
    UserSearchBST();
    ~UserSearchBST();
    
    // Main operations
    void insert(const SearchUser& user);
    std::vector<SearchUser> searchByPrefix(const std::string& prefix);
    std::vector<SearchUser> getAllUsers();
    void clear();
    void rebuild(const std::vector<SearchUser>& users);
};

// Friend Search Service
class FriendSearchService {
private:
    sqlite3* db;
    UserSearchBST searchTree;
    
    // Helper functions
    SearchUser createUserFromRow(sqlite3_stmt* stmt);
    void setFriendshipStatus(SearchUser& user, const std::string& currentUsername);
    
public:
    FriendSearchService(sqlite3* database);
    
    // Main search operations
    std::vector<SearchUser> searchUsersByPrefix(const std::string& prefix, 
                                               const std::string& currentUsername,
                                               int limit = 50);
    std::vector<SearchUser> searchUsersByEmail(const std::string& email, 
                                              const std::string& currentUsername);
    std::vector<SearchUser> getAllUsers(const std::string& currentUsername, int limit = 100);
    
    // Friend operations
    std::vector<SearchUser> getFriends(const std::string& username);
    std::vector<SearchUser> getFriendRequests(const std::string& username);
    std::vector<SearchUser> getSuggestedFriends(const std::string& username, int limit = 20);
    
    // Database operations
    bool sendFriendRequest(const std::string& requester, const std::string& addressee);
    bool acceptFriendRequest(const std::string& requester, const std::string& addressee);
    bool rejectFriendRequest(const std::string& requester, const std::string& addressee);
    bool removeFriend(const std::string& user1, const std::string& user2);
    
    // Utility functions
    void refreshSearchIndex();
    crow::json::wvalue userToJson(const SearchUser& user);
    std::vector<crow::json::wvalue> usersToJson(const std::vector<SearchUser>& users);
};

// HTTP Route handlers
crow::response handleSearchUsers(sqlite3* db, const crow::request& req);
crow::response handleGetFriends(sqlite3* db, const crow::request& req);
crow::response handleSendFriendRequest(sqlite3* db, const crow::request& req);
crow::response handleRespondToFriendRequest(sqlite3* db, const crow::request& req);
crow::response handleRemoveFriend(sqlite3* db, const crow::request& req);
crow::response handleGetFriendSuggestions(sqlite3* db, const crow::request& req);