#pragma once
#include <sqlite3.h>
#include <string>
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
bool removeFriend(sqlite3* db, const std::string& username, const std::string& friendUsername);
std::string getFriendsList(sqlite3* db, const std::string& username); // returns JSON string
// profile
std::string getProfile(sqlite3* db, const std::string& username); // returns JSON string
bool updateProfile(sqlite3* db, const std::string& username, const std::string& newProfileData);
// notifications
std::string getNotifications(sqlite3* db, const std::string& username); // returns JSON string
bool markNotificationAsRead(sqlite3* db, int notificationId);
