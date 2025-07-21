#pragma once
#include <sqlite3.h>
#include <string>
// login and signup
bool checkCredentials(sqlite3* db, const std::string& username, const std::string& password);
bool userExists(sqlite3* db, const std::string& username);
bool registerUser(sqlite3* db, const std::string& username, const std::string& password);
// Posts
bool createPost(sqlite3* db, const std::string& username, const std::string& content);
bool deletePost(sqlite3* db, int postId);
// Feed
bool getFeed(sqlite3* db, const std::string& username, std::string& feedData);
bool getUserPosts(sqlite3* db, const std::string& username, std::string& userPostsData);
// Likes
bool likePost(sqlite3* db, int postId, const std::string& username);
bool unlikePost(sqlite3* db, int postId, const std::string& username);
// Comments 
bool commentOnPost(sqlite3* db, int postId, const std::string& username, const std::string& comment);
bool deleteComment(sqlite3* db, int commentId);
// friends
bool addFriend(sqlite3* db, const std::string& username, const std::string& friendUsername);
bool removeFriend(sqlite3* db, const std::string& username, const std::string& friendUsername);
bool getFriendsList(sqlite3* db, const std::string& username, std::string& friendsListData);
// profile
bool getProfile(sqlite3* db, const std::string& username, std::string& profileData);
bool updateProfile(sqlite3* db, const std::string& username, const std::string& newProfileData);
// notifications
bool getNotifications(sqlite3* db, const std::string& username, std::string& notificationsData);
bool markNotificationAsRead(sqlite3* db, int notificationId);
