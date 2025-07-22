#pragma once
#include <sqlite3.h>
#include <string>
#include <vector>
#include <ctime>
#include <crow.h>

struct Notification {
    int id;
    std::string recipient_username;
    std::string sender_username;
    std::string type; // "friend_request", "post", "comment", "like"
    std::string content; // message to show
    std::string created_at; // timestamp
    bool is_read;
    std::string metadata; // JSON string for additional data
};

class NotificationService {
private:
    sqlite3* db;

public:
    NotificationService(sqlite3* database);
    
    // Core notification operations
    bool createNotification(const std::string& recipient, const std::string& sender, 
                          const std::string& type, const std::string& content, 
                          const std::string& metadata = "");
    
    std::vector<Notification> getNotificationsForUser(const std::string& username, 
                                                     bool unread_only = false, 
                                                     int limit = 50);
    
    bool markAsRead(int notification_id);
    bool markAllAsRead(const std::string& username);
    
    int getUnreadCount(const std::string& username);
    
    bool deleteNotification(int notification_id);
    bool deleteOldNotifications(int days_old = 30);
    
    // Specific notification types
    bool createFriendRequestNotification(const std::string& recipient, const std::string& sender);
    bool createNewPostNotification(const std::string& post_author, const std::string& post_content);
    bool createCommentNotification(const std::string& post_author, const std::string& commenter, 
                                 const std::string& post_id);
    bool createLikeNotification(const std::string& post_author, const std::string& liker, 
                              const std::string& post_id);
    
    // Database setup
    bool initializeDatabase();
    
    // Helper functions
    std::string getCurrentTimestamp();
    crow::json::wvalue notificationToJson(const Notification& notification);
    std::vector<crow::json::wvalue> notificationsToJson(const std::vector<Notification>& notifications);
};

// HTTP Route handlers for notifications
crow::response handleGetNotifications(sqlite3* db, const crow::request& req);
crow::response handleMarkAsRead(sqlite3* db, const crow::request& req);
crow::response handleGetUnreadCount(sqlite3* db, const crow::request& req);