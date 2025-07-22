#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
// login and signup
bool checkCredentials(sqlite3* db, const std::string& username, const std::string& password);
bool userExists(sqlite3* db, const std::string& username);
bool registerUser(sqlite3* db, const std::string& username, const std::string& password);
// Posts
int createPost(sqlite3* db, const std::string& username, const std::string& content); // returns postId
bool deletePost(sqlite3* db, int postId);
// Feed
std::string getFeed(sqlite3* db, const std::string& username); // returns JSON string
// std::string getUserPosts(sqlite3* db, const std::string& username); // returns JSON string
// Likes
// bool likePost(sqlite3* db, int postId, const std::string& username);
// bool unlikePost(sqlite3* db, int postId, const std::string& username);
// Comments 
// int commentOnPost(sqlite3* db, int postId, const std::string& username, const std::string& comment); // returns commentId
// bool deleteComment(sqlite3* db, int commentId);
// friends
bool addFriend(sqlite3* db, const std::string& username, const std::string& friendUsername);
bool removefriend(sqlite3* db, const std::string& username, const std::string& friendUsername);
std::string getFriendsList(sqlite3* db, const std::string& username); // returns JSON string
// profile
std::string getProfile(sqlite3* db, const std::string& username); // returns JSON string
bool updateProfile(sqlite3* db, const std::string& username, const std::string& newProfileData);
// notifications
std::string getNotifications(sqlite3* db, const std::string& username); // returns JSON string
bool markNotificationAsRead(sqlite3* db, int notificationId);
// Friend search helpers
std::vector<std::string> searchUsersByPrefix(sqlite3* db, const std::string& prefix, int limit = 50);
std::vector<std::string> getAllUsersExcept(sqlite3* db, const std::string& excludeUsername, int limit = 100);
bool areFriends(sqlite3* db, const std::string& user1, const std::string& user2);
bool hasPendingFriendRequest(sqlite3* db, const std::string& requester, const std::string& addressee);
std::string getUserInfo(sqlite3* db, const std::string& username); // returns JSON string with user details
bool updateUser(sqlite3* db, const std::string& user_id, const std::string& username, const std::string& password_hash, const std::string& profile_pic, const std::string& bio);
bool deleteUser(sqlite3* db, const std::string& user_id);
bool getUserByUsername(sqlite3* db, const std::string& username, std::string& user_id, std::string& password_hash, std::string& profile_pic, std::string& bio, time_t& created_at);
bool getUserByID(sqlite3* db, const std::string& user_id, std::string& username, std::string& password_hash, std::string& profile_pic, std::string& bio, time_t& created_at);