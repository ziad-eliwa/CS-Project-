// Include the header file that contains the NotificationService class declaration
#include "notification_service.h"
// Include login handler to access session management functions and active_sessions map
#include "../handlers/login_handler.h" // for session management
// Include iostream for console output (std::cout, std::cerr)
#include <iostream>
// Include sstream for string stream operations (used in timestamp formatting)
#include <sstream>
// Include iomanip for input/output manipulations (std::put_time for timestamp formatting)
#include <iomanip>

// Constructor for NotificationService class - takes a SQLite database pointer as parameter
// Uses member initializer list to set the db member variable to the passed database pointer
NotificationService::NotificationService(sqlite3* database) : db(database) {
    // Call initializeDatabase() to create the notifications table and indexes if they don't exist
    initializeDatabase();
}

// Function to initialize the database schema for notifications
// Returns true if successful, false if there's an error
bool NotificationService::initializeDatabase() {
    // Define SQL DDL (Data Definition Language) statement to create notifications table
    // Uses raw string literal R"(...)" to avoid escaping quotes
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS notifications (
            id INTEGER PRIMARY KEY AUTOINCREMENT,    -- Unique identifier for each notification
            recipient_username TEXT NOT NULL,        -- Username of the person receiving the notification
            sender_username TEXT,                     -- Username of the person who triggered the notification (can be NULL)
            type TEXT NOT NULL,                      -- Type of notification (friend_request, like, comment, etc.)
            content TEXT NOT NULL,                   -- Human-readable message content
            metadata TEXT DEFAULT '{}',              -- JSON string for additional data (default empty JSON object)
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,  -- When the notification was created
            is_read BOOLEAN DEFAULT 0,               -- Whether notification has been read (0=false, 1=true)
            FOREIGN KEY (recipient_username) REFERENCES users(username),  -- Ensure recipient exists in users table
            FOREIGN KEY (sender_username) REFERENCES users(username)      -- Ensure sender exists in users table
        );
        
        -- Create database indexes for faster query performance
        CREATE INDEX IF NOT EXISTS idx_notifications_recipient ON notifications(recipient_username);  -- Fast lookup by recipient
        CREATE INDEX IF NOT EXISTS idx_notifications_read ON notifications(is_read);                  -- Fast filtering by read status
        CREATE INDEX IF NOT EXISTS idx_notifications_created ON notifications(created_at);            -- Fast ordering by creation time
    )";
    
    // Pointer to hold any SQL error messages
    char* errMsg = 0;
    // Execute the SQL statement - returns SQLITE_OK (0) on success
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    
    // Check if SQL execution failed
    if (rc != SQLITE_OK) {
        // Print error message to console error stream
        std::cerr << "SQL error creating notifications table: " << errMsg << std::endl;
        // Free the error message memory allocated by SQLite
        sqlite3_free(errMsg);
        // Return false to indicate failure
        return false;
    }
    
    // Return true to indicate successful table/index creation
    return true;
}

// Helper function to get the current timestamp in "YYYY-MM-DD HH:MM:SS" format
// Returns a string representation of the current date and time
std::string NotificationService::getCurrentTimestamp() {
    // Get current time as time_t (seconds since Unix epoch)
    auto t = std::time(nullptr);
    // Convert to local time structure (tm) containing year, month, day, hour, minute, second
    auto tm = *std::localtime(&t);
    // Create output string stream for formatting
    std::ostringstream oss;
    // Format the time using put_time manipulator with specified format string
    // %Y = 4-digit year, %m = month, %d = day, %H = hour (24h), %M = minute, %S = second
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    // Convert string stream to string and return
    return oss.str();
}

// Function to create a new notification in the database
// Parameters: recipient (who gets it), sender (who triggered it), type, content message, metadata (JSON)
// Returns true if notification was successfully created, false if error occurred
bool NotificationService::createNotification(const std::string& recipient, const std::string& sender, 
                                           const std::string& type, const std::string& content, 
                                           const std::string& metadata) {
    // SQL INSERT statement using prepared statement placeholders (?) for security
    const char* sql = R"(
        INSERT INTO notifications (recipient_username, sender_username, type, content, metadata)
        VALUES (?, ?, ?, ?, ?);
    )";
    
    // Pointer to hold the prepared SQL statement
    sqlite3_stmt* stmt;
    // Prepare the SQL statement for execution - compiles SQL and returns status code
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    // Check if statement preparation failed
    if (rc != SQLITE_OK) {
        // Print error message with SQLite's error description
        std::cerr << "Failed to prepare notification insert statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Bind parameter values to the prepared statement placeholders
    sqlite3_bind_text(stmt, 1, recipient.c_str(), -1, SQLITE_STATIC);  // Bind recipient to first ?
    sqlite3_bind_text(stmt, 2, sender.c_str(), -1, SQLITE_STATIC);     // Bind sender to second ?
    sqlite3_bind_text(stmt, 3, type.c_str(), -1, SQLITE_STATIC);       // Bind type to third ?
    sqlite3_bind_text(stmt, 4, content.c_str(), -1, SQLITE_STATIC);    // Bind content to fourth ?
    sqlite3_bind_text(stmt, 5, metadata.c_str(), -1, SQLITE_STATIC);   // Bind metadata to fifth ?
    // Note: -1 means SQLite will calculate string length, SQLITE_STATIC means string won't change
    
    // Execute the prepared statement
    rc = sqlite3_step(stmt);
    // Clean up the prepared statement to free memory
    sqlite3_finalize(stmt);
    
    // Check if execution failed (SQLITE_DONE means successful INSERT)
    if (rc != SQLITE_DONE) {
        // Print error message if INSERT failed
        std::cerr << "Failed to insert notification: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Return true to indicate successful notification creation
    return true;
}

// Function to retrieve notifications for a specific user from the database
// Parameters: username (whose notifications to get), unread_only (filter), limit (max results)
// Returns vector of Notification objects containing all matching notifications
std::vector<Notification> NotificationService::getNotificationsForUser(const std::string& username, 
                                                                       bool unread_only, int limit) {
    // Initialize empty vector to store results
    std::vector<Notification> notifications;
    
    // Base SQL SELECT statement to get all notification fields
    std::string sql = R"(
        SELECT id, recipient_username, sender_username, type, content, metadata, 
               created_at, is_read
        FROM notifications 
        WHERE recipient_username = ?
    )";
    
    // Conditionally add filter for unread notifications only
    if (unread_only) {
        sql += " AND is_read = 0";  // 0 = false (unread)
    }
    
    // Add ordering and limit to the query
    sql += " ORDER BY created_at DESC LIMIT ?";  // Most recent first, limited results
    
    // Prepare the dynamic SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL);
    
    // Check if statement preparation failed
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare notification select statement: " << sqlite3_errmsg(db) << std::endl;
        return notifications;  // Return empty vector on error
    }
    
    // Bind parameters to the prepared statement
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  // Username filter
    sqlite3_bind_int(stmt, 2, limit);                                 // Result limit
    
    // Execute query and process each result row
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Create new Notification object for this row
        Notification notification;
        
        // Extract data from each column (0-indexed)
        notification.id = sqlite3_column_int(stmt, 0);                                    // Column 0: id
        notification.recipient_username = (char*)sqlite3_column_text(stmt, 1);           // Column 1: recipient_username
        
        // Handle potentially NULL sender field
        const char* sender = (char*)sqlite3_column_text(stmt, 2);                        // Column 2: sender_username
        notification.sender_username = sender ? sender : "";                             // Use empty string if NULL
        
        notification.type = (char*)sqlite3_column_text(stmt, 3);                         // Column 3: type
        notification.content = (char*)sqlite3_column_text(stmt, 4);                      // Column 4: content
        
        // Handle potentially NULL metadata field
        const char* metadata = (char*)sqlite3_column_text(stmt, 5);                      // Column 5: metadata
        notification.metadata = metadata ? metadata : "{}";                              // Use empty JSON if NULL
        
        notification.created_at = (char*)sqlite3_column_text(stmt, 6);                   // Column 6: created_at
        notification.is_read = sqlite3_column_int(stmt, 7) == 1;                         // Column 7: is_read (convert int to bool)
        
        // Add the completed notification object to results vector
        notifications.push_back(notification);
    }
    
    // Clean up the prepared statement
    sqlite3_finalize(stmt);
    // Return vector containing all retrieved notifications
    return notifications;
}

// Function to mark a specific notification as read in the database
// Parameter: notification_id (unique ID of the notification to mark as read)
// Returns true if successfully updated, false if error occurred
bool NotificationService::markAsRead(int notification_id) {
    // SQL UPDATE statement to set is_read flag to 1 (true) for specific notification
    const char* sql = "UPDATE notifications SET is_read = 1 WHERE id = ?";
    
    // Prepare the SQL statement for execution
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    // Check if statement preparation failed
    if (rc != SQLITE_OK) {
        return false;  // Return false on preparation error
    }
    
    // Bind the notification ID parameter to the prepared statement
    sqlite3_bind_int(stmt, 1, notification_id);
    // Execute the UPDATE statement
    rc = sqlite3_step(stmt);
    // Clean up the prepared statement
    sqlite3_finalize(stmt);
    
    // Return true if UPDATE was successful (SQLITE_DONE means successful DML operation)
    return rc == SQLITE_DONE;
}

// Function to mark ALL notifications as read for a specific user
// Parameter: username (whose notifications to mark as read)
// Returns true if successfully updated, false if error occurred
bool NotificationService::markAllAsRead(const std::string& username) {
    // SQL UPDATE statement to set is_read flag to 1 for all notifications of a user
    const char* sql = "UPDATE notifications SET is_read = 1 WHERE recipient_username = ?";
    
    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    // Check if statement preparation failed
    if (rc != SQLITE_OK) {
        return false;
    }
    
    // Bind the username parameter
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    // Execute the UPDATE statement
    rc = sqlite3_step(stmt);
    // Clean up the prepared statement
    sqlite3_finalize(stmt);
    
    // Return true if UPDATE was successful
    return rc == SQLITE_DONE;
}

// Function to count how many unread notifications a user has
// Parameter: username (whose unread notifications to count)
// Returns integer count of unread notifications (0 if none or error)
int NotificationService::getUnreadCount(const std::string& username) {
    // SQL SELECT with COUNT aggregate function to count unread notifications
    const char* sql = "SELECT COUNT(*) FROM notifications WHERE recipient_username = ? AND is_read = 0";
    
    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    // Return 0 if statement preparation failed
    if (rc != SQLITE_OK) {
        return 0;
    }
    
    // Bind the username parameter to the query
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    
    // Initialize count to 0
    int count = 0;
    // Execute query and get result if successful
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);  // Get COUNT result from first column
    }
    
    // Clean up the prepared statement
    sqlite3_finalize(stmt);
    // Return the count of unread notifications
    return count;
}

// Function to permanently delete a specific notification from database
// Parameter: notification_id (unique ID of notification to delete)
// Returns true if successfully deleted, false if error occurred
bool NotificationService::deleteNotification(int notification_id) {
    // SQL DELETE statement to remove notification by ID
    const char* sql = "DELETE FROM notifications WHERE id = ?";
    
    // Prepare the SQL statement
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    
    // Return false if statement preparation failed
    if (rc != SQLITE_OK) {
        return false;
    }
    
    // Bind the notification ID parameter
    sqlite3_bind_int(stmt, 1, notification_id);
    // Execute the DELETE statement
    rc = sqlite3_step(stmt);
    // Clean up the prepared statement
    sqlite3_finalize(stmt);
    
    // Return true if DELETE was successful
    return rc == SQLITE_DONE;
}

// ========== Specific notification creator functions ==========
// These are convenience functions that create notifications for common social media events

// Function to create a friend request notification
// Parameters: recipient (who receives the request), sender (who sent the request)
// Returns true if notification was successfully created
bool NotificationService::createFriendRequestNotification(const std::string& recipient, const std::string& sender) {
    // Create human-readable message content
    std::string content = sender + " sent you a friend request";
    // Create JSON metadata with additional information about this notification type
    std::string metadata = R"({"type": "friend_request", "action_required": true})";
    // Call the general createNotification function with friend_request type
    return createNotification(recipient, sender, "friend_request", content, metadata);
}

// Function to create a new post notification (for notifying friends about new posts)
// Parameters: post_author (who created the post), post_content (content of the post)
// Returns true if notification was successfully created
bool NotificationService::createNewPostNotification(const std::string& post_author, const std::string& post_content) {
    // This would notify friends of the post author
    std::string content = post_author + " created a new post";
    // Create metadata with post preview (first 50 characters) embedded in JSON
    std::string metadata = R"({"type": "new_post", "post_preview": ")" + post_content.substr(0, 50) + R"("})";
    
    // For now, we'll create a notification for all users (in a real app, this would be for friends only)
    // You would need to implement a friends table and query it here
    return createNotification("all_users", post_author, "new_post", content, metadata);
}

// Function to create a comment notification (when someone comments on a post)
// Parameters: post_author (owner of the post), commenter (who made the comment), post_id (which post)
// Returns true if notification was successfully created
bool NotificationService::createCommentNotification(const std::string& post_author, const std::string& commenter, 
                                                   const std::string& post_id) {
    // Create message indicating someone commented on their post
    std::string content = commenter + " commented on your post";
    // Create metadata with post ID for reference (useful for navigation)
    std::string metadata = R"({"type": "comment", "post_id": ")" + post_id + R"("})";
    // Create notification for the post author
    return createNotification(post_author, commenter, "comment", content, metadata);
}

// Function to create a like notification (when someone likes a post)
// Parameters: post_author (owner of the post), liker (who liked the post), post_id (which post)
// Returns true if notification was successfully created
bool NotificationService::createLikeNotification(const std::string& post_author, const std::string& liker, 
                                                const std::string& post_id) {
    // Create message indicating someone liked their post
    std::string content = liker + " liked your post";
    // Create metadata with post ID for reference
    std::string metadata = R"({"type": "like", "post_id": ")" + post_id + R"("})";
    // Create notification for the post author
    return createNotification(post_author, liker, "like", content, metadata);
}

// ========== JSON conversion helper functions ==========
// These functions convert C++ objects to JSON format for HTTP responses

// Function to convert a single Notification object to JSON format
// Parameter: notification (Notification struct to convert)
// Returns crow::json::wvalue (JSON object) containing all notification fields
crow::json::wvalue NotificationService::notificationToJson(const Notification& notification) {
    // Create new JSON object
    crow::json::wvalue json;
    // Copy all notification fields to JSON properties
    json["id"] = notification.id;                           // Unique notification ID
    json["recipient"] = notification.recipient_username;    // Who receives the notification
    json["sender"] = notification.sender_username;          // Who triggered the notification
    json["type"] = notification.type;                       // Type of notification
    json["content"] = notification.content;                 // Human-readable message
    json["metadata"] = notification.metadata;               // Additional JSON data
    json["created_at"] = notification.created_at;           // When it was created
    json["is_read"] = notification.is_read;                 // Read status (boolean)
    // Return the populated JSON object
    return json;
}

// Function to convert a vector of Notification objects to JSON array format
// Parameter: notifications (vector of Notification structs)
// Returns vector of crow::json::wvalue objects (JSON array)
std::vector<crow::json::wvalue> NotificationService::notificationsToJson(const std::vector<Notification>& notifications) {
    // Initialize empty vector to hold JSON objects
    std::vector<crow::json::wvalue> json_notifications;
    // Convert each notification to JSON and add to vector
    for (const auto& notification : notifications) {
        json_notifications.push_back(notificationToJson(notification));
    }
    // Return vector of JSON objects
    return json_notifications;
}

// These functions handle incoming HTTP requests for notification-related endpoints

// Function to handle GET requests for retrieving user notifications
// Parameter: db (database connection), req (HTTP request object)
// Returns crow::response with JSON containing notifications or error
crow::response handleGetNotifications(sqlite3* db, const crow::request& req) {
    // Extract session ID from request cookies to identify the user
    std::string session_id = get_session_from_cookie(req);
    // Check if session exists in active_sessions map (user is logged in)
    if (active_sessions.find(session_id) == active_sessions.end()) {
        // Return 401 Unauthorized if session is invalid
        return crow::response(401, "Unauthorized");
    }
    
    // Get username from the active session
    std::string username = active_sessions[session_id];
    // Create NotificationService instance with database connection
    NotificationService notificationService(db);
    
    // Parse query parameters from URL
    // Check if "unread" parameter exists (for filtering unread notifications only)
    bool unread_only = req.url_params.get("unread") != nullptr;
    // Get limit parameter or default to 50 notifications
    int limit = 50;
    if (req.url_params.get("limit")) {
        limit = std::stoi(req.url_params.get("limit"));  // Convert string to integer
    }
    
    // Retrieve notifications from database using the service
    auto notifications = notificationService.getNotificationsForUser(username, unread_only, limit);
    
    // Create JSON response object
    crow::json::wvalue response;
    response["notifications"] = crow::json::wvalue::list();  // Initialize empty JSON array
    
    // Convert each notification to JSON and add to response array
    for (size_t i = 0; i < notifications.size(); ++i) {
        response["notifications"][i] = notificationService.notificationToJson(notifications[i]);
    }
    
    // Add metadata to response
    response["count"] = static_cast<int>(notifications.size());           // Number of notifications returned
    response["unread_count"] = notificationService.getUnreadCount(username);  // Total unread count for badge
    
    // Return HTTP 200 OK with JSON response
    return crow::response(200, response);
}

// Function to handle POST requests for marking notifications as read
// Parameter: db (database connection), req (HTTP request object)
// Returns crow::response indicating success or failure
crow::response handleMarkAsRead(sqlite3* db, const crow::request& req) {
    // Verify user authentication via session cookie
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
        return crow::response(401, "Unauthorized");
    }
    
    // Parse JSON request body to get action parameters
    auto body = crow::json::load(req.body);
    if (!body) {
        return crow::response(400, "Invalid JSON");  // Return 400 Bad Request for malformed JSON
    }
    
    // Create NotificationService instance
    NotificationService notificationService(db);
    
    // Check if request is to mark a specific notification as read
    if (body.has("notification_id")) {
        // Extract notification ID from JSON body
        int notification_id = body["notification_id"].i();
        // Attempt to mark the specific notification as read
        if (notificationService.markAsRead(notification_id)) {
            // Return success response if operation succeeded
            return crow::response(200, R"({"success": true, "message": "Notification marked as read"})");
        }
    } 
    // Check if request is to mark ALL notifications as read for current user
    else if (body.has("mark_all")) {
        // Get current user's username from session
        std::string username = active_sessions[session_id];
        // Attempt to mark all notifications as read for this user
        if (notificationService.markAllAsRead(username)) {
            // Return success response if operation succeeded
            return crow::response(200, R"({"success": true, "message": "All notifications marked as read"})");
        }
    }
    
    // Return error response if neither operation succeeded
    return crow::response(500, R"({"success": false, "message": "Failed to mark as read"})");
}

// Function to handle GET requests for retrieving unread notification count
// Used for notification badges in UI to show number of unread notifications
// Parameter: db (database connection), req (HTTP request object)  
// Returns crow::response with JSON containing unread count
crow::response handleGetUnreadCount(sqlite3* db, const crow::request& req) {
    // Verify user authentication via session cookie
    std::string session_id = get_session_from_cookie(req);
    if (active_sessions.find(session_id) == active_sessions.end()) {
        return crow::response(401, "Unauthorized");  // Return 401 if not logged in
    }
    
    // Get current user's username from the authenticated session
    std::string username = active_sessions[session_id];
    // Create NotificationService instance to interact with database
    NotificationService notificationService(db);
    
    // Query database for count of unread notifications for this user
    int unread_count = notificationService.getUnreadCount(username);
    
    // Create JSON response with the unread count
    crow::json::wvalue response;
    response["unread_count"] = unread_count;  // Add count to JSON response
    
    // Return HTTP 200 OK with JSON containing unread count
    return crow::response(200, response);
}