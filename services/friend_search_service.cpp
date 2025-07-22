// Header file inclusion section - importing necessary dependencies and libraries
#include "friend_search_service.h"        // Main header file defining the FriendSearchService class and structures
#include "../handlers/login_handler.h"    // For session management functions (get_session_from_cookie, active_sessions)
#include "../database/db_utils.h"         // Database utility functions for SQLite operations
#include <algorithm>                      // STL algorithms like std::remove_if, std::transform for data manipulation
#include <iostream>                       // Input/output stream operations for debugging and logging
#include <sstream>                        // String stream operations for string manipulation
#include <cctype>                         // Character classification functions like ::tolower for case conversion

// Binary Search Tree (BST) Implementation for efficient user search operations
// The BST is used to store and search users by username with O(log n) average complexity

// Default constructor - initializes an empty BST with root pointer set to nullptr
UserSearchBST::UserSearchBST() : root(nullptr) {}

// Destructor - ensures proper cleanup of all nodes to prevent memory leaks
UserSearchBST::~UserSearchBST() {
    clear();  // Call clear() to recursively delete all nodes in the tree
}

// Recursive helper function to insert a new user node into the BST
// Parameters: node (current node in recursion), user (SearchUser object to insert)
// Returns: shared_ptr to the node (for tree reconstruction during recursion)
std::shared_ptr<BSTNode> UserSearchBST::insertNode(std::shared_ptr<BSTNode> node, const SearchUser& user) {
    // Base case: if current node is null, create and return new node
    if (!node) {
        return std::make_shared<BSTNode>(user);  // Create new BSTNode with SearchUser data
    }
    
    // Extract usernames for comparison - prepare for case-insensitive comparison
    std::string nodeUsername = node->user.username;    // Username from current node
    std::string insertUsername = user.username;        // Username from user to insert
    
    // Convert both usernames to lowercase for case-insensitive comparison
    // This ensures "Alice" and "alice" are treated as equivalent for ordering
    std::transform(nodeUsername.begin(), nodeUsername.end(), nodeUsername.begin(), ::tolower);
    std::transform(insertUsername.begin(), insertUsername.end(), insertUsername.begin(), ::tolower);
    
    // BST insertion logic: compare usernames to determine left or right subtree placement
    if (insertUsername < nodeUsername) {
        // Insert username is lexicographically smaller - go to left subtree
        node->left = insertNode(node->left, user);
    } else if (insertUsername > nodeUsername) {
        // Insert username is lexicographically larger - go to right subtree
        node->right = insertNode(node->right, user);
    }
    // If usernames are equal (insertUsername == nodeUsername), don't insert duplicate
    // The function implicitly handles duplicates by doing nothing in this case
    
    // Return the current node (unchanged) to maintain tree structure during recursion
    return node;
}

// Public interface method to insert a user into the BST
// Parameter: user (SearchUser object containing user data to be inserted)
// This method provides a clean interface by calling the recursive insertNode helper
void UserSearchBST::insert(const SearchUser& user) {
    root = insertNode(root, user);  // Update root with result of recursive insertion
}

// Recursive inorder traversal function to retrieve all users in alphabetical order
// Parameters: node (current node in traversal), result (vector to store users in sorted order)
// Inorder traversal (Left -> Root -> Right) produces alphabetically sorted usernames
void UserSearchBST::inorderTraversal(std::shared_ptr<BSTNode> node, std::vector<SearchUser>& result) {
    if (!node) return;  // Base case: null node means end of this branch
    
    // Recursive inorder traversal pattern:
    inorderTraversal(node->left, result);   // 1. Process left subtree first (smaller usernames)
    result.push_back(node->user);           // 2. Process current node (add to result)
    inorderTraversal(node->right, result);  // 3. Process right subtree last (larger usernames)
}

// Recursive function to search for users whose usernames start with a given prefix
// Parameters: node (current node), prefix (search prefix), result (vector to store matching users)
// This implements a prefix search algorithm optimized for BST structure
void UserSearchBST::prefixSearch(std::shared_ptr<BSTNode> node, const std::string& prefix, 
                                std::vector<SearchUser>& result) {
    if (!node) return;  // Base case: null node means end of this branch
    
    // Prepare strings for case-insensitive comparison
    std::string nodeUsername = node->user.username;  // Username from current node
    std::string searchPrefix = prefix;               // Search prefix from parameter
    
    // Convert both strings to lowercase for case-insensitive prefix matching
    // This allows "al" to match "Alice", "ALICE", "alice", etc.
    std::transform(nodeUsername.begin(), nodeUsername.end(), nodeUsername.begin(), ::tolower);
    std::transform(searchPrefix.begin(), searchPrefix.end(), searchPrefix.begin(), ::tolower);
    
    // Check if current node's username starts with the search prefix
    // substr(0, searchPrefix.length()) extracts the first N characters where N = prefix length
    if (nodeUsername.substr(0, searchPrefix.length()) == searchPrefix) {
        result.push_back(node->user);  // Add matching user to results
    }
    
    // Recursively search both subtrees because prefix matches can exist in both directions
    // Unlike exact search, prefix search requires checking both sides of the tree
    // since "al" could match "alice" (left) and "alex" (right) relative to "bob"
    if (searchPrefix <= nodeUsername) {
        prefixSearch(node->left, prefix, result);   // Search left subtree for smaller usernames
    }
    if (searchPrefix >= nodeUsername.substr(0, searchPrefix.length())) {
        prefixSearch(node->right, prefix, result);  // Search right subtree for larger usernames
    }
}

// Public method to search for users by username prefix
// Parameter: prefix (string that usernames should start with)
// Returns: vector of SearchUser objects whose usernames begin with the prefix
std::vector<SearchUser> UserSearchBST::searchByPrefix(const std::string& prefix) {
    std::vector<SearchUser> result;  // Initialize empty result vector
    if (prefix.empty()) {
        // If no prefix is provided, return all users in alphabetical order
        // This is useful for "browse all users" functionality
        return getAllUsers();
    }
    // Call recursive prefix search starting from root node
    prefixSearch(root, prefix, result);
    return result;  // Return vector containing all matching users
}

// Public method to retrieve all users stored in the BST in alphabetical order
// Returns: vector of all SearchUser objects sorted by username (case-insensitive)
std::vector<SearchUser> UserSearchBST::getAllUsers() {
    std::vector<SearchUser> result;  // Initialize empty result vector
    // Perform inorder traversal starting from root to get all users in sorted order
    inorderTraversal(root, result);
    return result;  // Return alphabetically sorted list of all users
}

// Recursive helper function to delete all nodes in the BST (post-order traversal)
// Parameter: node (current node to delete)
// Uses post-order traversal (Left -> Right -> Root) to safely delete all child nodes before parent
void UserSearchBST::clearTree(std::shared_ptr<BSTNode> node) {
    if (!node) return;  // Base case: null node means nothing to delete
    clearTree(node->left);   // Recursively delete left subtree first
    clearTree(node->right);  // Recursively delete right subtree second
    // Node automatically deleted when shared_ptr goes out of scope (RAII)
}

// Public method to clear all nodes from the BST and reset to empty state
void UserSearchBST::clear() {
    clearTree(root);     // Delete all nodes starting from root
    root = nullptr;      // Reset root pointer to null (empty tree state)
}

// Public method to rebuild the entire BST with a new set of users
// Parameter: users (vector of SearchUser objects to populate the tree)
// This is useful for refreshing the search index when user data changes
void UserSearchBST::rebuild(const std::vector<SearchUser>& users) {
    clear();  // First, remove all existing nodes from the tree
    // Insert each user from the vector into the empty BST
    for (const auto& user : users) {
        insert(user);  // Add user to BST using standard insertion method
    }
}

// FriendSearchService Implementation - High-level service for user search and friend management
// This class provides the main business logic for user search functionality

// Constructor: initializes the service with database connection and builds initial search index
// Parameter: database (pointer to SQLite database connection)
FriendSearchService::FriendSearchService(sqlite3* database) : db(database) {
    refreshSearchIndex();  // Build BST search index from current database users
}

// Helper method to create a SearchUser object from SQLite query result row
// Parameter: stmt (prepared SQLite statement with executed query result)
// Returns: SearchUser object populated with data from current row
SearchUser FriendSearchService::createUserFromRow(sqlite3_stmt* stmt) {
    SearchUser user;  // Create empty SearchUser object
    
    // Extract user ID from column 0 (integer type)
    user.id = sqlite3_column_int(stmt, 0);
    
    // Extract username from column 1, handle potential NULL values
    const char* username = (char*)sqlite3_column_text(stmt, 1);
    user.username = username ? username : "";  // Use empty string if NULL
    
    // Set email to empty string since it doesn't exist in database
    user.email = "";
    
    // Extract profile picture URL from column 2, handle potential NULL values
    const char* profile_pic = (char*)sqlite3_column_text(stmt, 2);
    user.profile_pic = profile_pic ? profile_pic : "";  // Use empty string if NULL
    
    // Extract user bio from column 3, handle potential NULL values
    const char* bio = (char*)sqlite3_column_text(stmt, 3);
    user.bio = bio ? bio : "";  // Use empty string if NULL
    
    // Extract account creation timestamp from column 4, handle potential NULL values
    const char* created_at = (char*)sqlite3_column_text(stmt, 4);
    user.created_at = created_at ? created_at : "";  // Use empty string if NULL
    
    return user;  // Return fully populated SearchUser object
}

// Method to determine and set friendship status between current user and target user
// Parameters: user (SearchUser object to update), currentUsername (logged-in user's username)
// This method queries the database to check if users are friends or have pending requests
void FriendSearchService::setFriendshipStatus(SearchUser& user, const std::string& currentUsername) {
    // SQL query to check if two users are friends (bidirectional friendship)
    // Uses JOIN to connect friends table with users table to match usernames
    // Checks both directions: (current->target) OR (target->current) with 'accepted' status
    const char* friendQuery = R"(
        SELECT COUNT(*) FROM friends f
        JOIN users u1 ON f.requester_id = u1.id
        JOIN users u2 ON f.addressee_id = u2.id
        WHERE f.status = 'accepted' AND (
            (u1.username = ? AND u2.username = ?) OR
            (u1.username = ? AND u2.username = ?)
        )
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the friendship check query
    if (sqlite3_prepare_v2(db, friendQuery, -1, &stmt, NULL) == SQLITE_OK) {
        // Bind parameters: check both directions of friendship
        sqlite3_bind_text(stmt, 1, currentUsername.c_str(), -1, SQLITE_STATIC);  // u1.username = currentUsername
        sqlite3_bind_text(stmt, 2, user.username.c_str(), -1, SQLITE_STATIC);   // u2.username = user.username
        sqlite3_bind_text(stmt, 3, user.username.c_str(), -1, SQLITE_STATIC);   // u1.username = user.username
        sqlite3_bind_text(stmt, 4, currentUsername.c_str(), -1, SQLITE_STATIC); // u2.username = currentUsername
        
        // Execute query and check result
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // If COUNT(*) > 0, users are friends; otherwise they are not
            user.is_friend = sqlite3_column_int(stmt, 0) > 0;
        }
        sqlite3_finalize(stmt);  // Clean up prepared statement
    }
    
    // SQL query to check for pending friend requests between users
    // Similar structure but checks for 'pending' status instead of 'accepted'
    const char* pendingQuery = R"(
        SELECT COUNT(*) FROM friends f
        JOIN users u1 ON f.requester_id = u1.id
        JOIN users u2 ON f.addressee_id = u2.id
        WHERE f.status = 'pending' AND (
            (u1.username = ? AND u2.username = ?) OR
            (u1.username = ? AND u2.username = ?)
        )
    )";
    
    // Prepare the pending request check query
    if (sqlite3_prepare_v2(db, pendingQuery, -1, &stmt, NULL) == SQLITE_OK) {
        // Bind parameters: check both directions of pending requests
        sqlite3_bind_text(stmt, 1, currentUsername.c_str(), -1, SQLITE_STATIC);  // current sent request to user
        sqlite3_bind_text(stmt, 2, user.username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, user.username.c_str(), -1, SQLITE_STATIC);   // user sent request to current
        sqlite3_bind_text(stmt, 4, currentUsername.c_str(), -1, SQLITE_STATIC);
        
        // Execute query and check result
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // If COUNT(*) > 0, there is a pending request between users
            user.has_pending_request = sqlite3_column_int(stmt, 0) > 0;
        }
        sqlite3_finalize(stmt);  // Clean up prepared statement
    }
}

// Public method to search for users by username prefix using the BST index
// Parameters: prefix (string to search for), currentUsername (logged-in user), limit (max results)
// Returns: vector of SearchUser objects matching the prefix with friendship status information
std::vector<SearchUser> FriendSearchService::searchUsersByPrefix(const std::string& prefix, 
                                                                const std::string& currentUsername,
                                                                int limit) {
    // Use BST to efficiently search for users with matching username prefix
    auto results = searchTree.searchByPrefix(prefix);
    
    // Remove current user from search results using std::remove_if algorithm
    // This prevents users from finding themselves in search results
    results.erase(std::remove_if(results.begin(), results.end(),
        [&currentUsername](const SearchUser& user) {
            return user.username == currentUsername;  // Lambda function to identify current user
        }), results.end());
    
    // Set friendship status for each user in the results
    // This adds is_friend and has_pending_request information to each SearchUser
    for (auto& user : results) {
        setFriendshipStatus(user, currentUsername);
    }
    
    // Limit the number of results to prevent overwhelming the UI
    // Only resize if we have more results than the requested limit
    if (results.size() > static_cast<size_t>(limit)) {
        results.resize(limit);  // Truncate to requested limit
    }
    
    return results;  // Return processed and limited search results
}

// Public method to search for users by email address using direct database query
// Parameters: email (email pattern to search for), currentUsername (logged-in user)
// Returns: vector of SearchUser objects with matching email addresses
std::vector<SearchUser> FriendSearchService::searchUsersByEmail(const std::string& email, 
                                                               const std::string& currentUsername) {
    std::vector<SearchUser> results;  // Initialize empty results vector
    
    // SQL query to search users by email using LIKE pattern matching
    // Uses ORDER BY username for consistent ordering and LIMIT 20 for performance
    const char* sql = R"(
        SELECT id, username, profile_pic, bio, created_at 
        FROM users 
        WHERE username LIKE ? AND username != ?
        ORDER BY username LIMIT 20
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the email search query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        // Create LIKE pattern with wildcards: %email% matches any string containing email
        std::string emailPattern = "%" + email + "%";
        sqlite3_bind_text(stmt, 1, emailPattern.c_str(), -1, SQLITE_STATIC);    // Email pattern
        sqlite3_bind_text(stmt, 2, currentUsername.c_str(), -1, SQLITE_STATIC); // Exclude current user
        
        // Execute query and process all matching rows
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            SearchUser user = createUserFromRow(stmt);          // Create SearchUser from row data
            setFriendshipStatus(user, currentUsername);         // Add friendship status information
            results.push_back(user);                            // Add to results vector
        }
        sqlite3_finalize(stmt);  // Clean up prepared statement
    }
    
    return results;  // Return all matching users with friendship status
}

// Public method to get all users in the system with friendship status information
// Parameters: currentUsername (logged-in user), limit (maximum number of users to return)
// Returns: vector of all SearchUser objects (excluding current user) with friendship status
std::vector<SearchUser> FriendSearchService::getAllUsers(const std::string& currentUsername, int limit) {
    // Get all users from BST in alphabetical order
    auto results = searchTree.getAllUsers();
    
    // Remove current user from the complete user list
    // This prevents the user from seeing themselves in the "all users" list
    results.erase(std::remove_if(results.begin(), results.end(),
        [&currentUsername](const SearchUser& user) {
            return user.username == currentUsername;  // Filter out current user
        }), results.end());
    
    // Set friendship status for each user in the complete list
    // This is computationally expensive for large user bases but provides complete information
    for (auto& user : results) {
        setFriendshipStatus(user, currentUsername);
    }
    
    // Apply limit to prevent overwhelming the client with too many results
    // Useful for pagination or initial page loads
    if (results.size() > static_cast<size_t>(limit)) {
        results.resize(limit);  // Truncate to requested limit
    }
    
    return results;  // Return limited list of all users with friendship status
}

// Public method to retrieve all friends of a specific user
// Parameter: username (username of the user whose friends to retrieve)
// Returns: vector of SearchUser objects representing all friends of the specified user
std::vector<SearchUser> FriendSearchService::getFriends(const std::string& username) {
    std::vector<SearchUser> friends;  // Initialize empty friends vector
    
    // Complex SQL query to find all accepted friendships for a user
    // Uses multiple JOINs to connect friends table with users table
    // Handles bidirectional friendship: user can be either requester or addressee
    const char* sql = R"(
        SELECT u.id, u.username, u.profile_pic, u.bio, u.created_at
        FROM users u
        JOIN friends f ON (u.id = f.requester_id OR u.id = f.addressee_id)
        JOIN users current ON (current.id = f.requester_id OR current.id = f.addressee_id)
        WHERE current.username = ? AND u.username != ? AND f.status = 'accepted'
        ORDER BY u.username
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the friends retrieval query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  // Find friendships for this user
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);  // Exclude the user themselves
        
        // Process all friends found in the query
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            SearchUser user = createUserFromRow(stmt);  // Create SearchUser from database row
            user.is_friend = true;                      // Mark as friend (since query only returns friends)
            friends.push_back(user);                    // Add to friends list
        }
        sqlite3_finalize(stmt);  // Clean up prepared statement
    }
    
    return friends;  // Return complete list of user's friends
}

// Public method to retrieve all pending friend requests received by a user
// Parameter: username (username of the user whose pending requests to retrieve)
// Returns: vector of SearchUser objects representing users who sent friend requests
std::vector<SearchUser> FriendSearchService::getFriendRequests(const std::string& username) {
    std::vector<SearchUser> requests;  // Initialize empty requests vector
    
    // SQL query to find all pending friend requests where user is the addressee (recipient)
    // Orders by creation timestamp (most recent first) for better user experience
    const char* sql = R"(
        SELECT u.id, u.username, u.profile_pic, u.bio, u.created_at
        FROM users u
        JOIN friends f ON u.id = f.requester_id
        JOIN users addressee ON addressee.id = f.addressee_id
        WHERE addressee.username = ? AND f.status = 'pending'
        ORDER BY f.created_at DESC
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the friend requests retrieval query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  // Find requests for this user
        
        // Process all pending requests found in the query
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            SearchUser user = createUserFromRow(stmt);      // Create SearchUser from database row
            user.has_pending_request = true;                // Mark as having pending request
            requests.push_back(user);                       // Add to requests list
        }
        sqlite3_finalize(stmt);  // Clean up prepared statement
    }
    
    return requests;  // Return list of users who sent friend requests
}

// Public method to get suggested friends for a user using a simple algorithm
// Parameters: username (user to get suggestions for), limit (maximum suggestions to return)
// Returns: vector of SearchUser objects representing potential friends
std::vector<SearchUser> FriendSearchService::getSuggestedFriends(const std::string& username, int limit) {
    std::vector<SearchUser> suggestions;  // Initialize empty suggestions vector
    
    // Simple friend suggestion algorithm: find users who are NOT friends and have NO pending requests
    // This excludes users who are already connected or have ongoing friend request interactions
    // More sophisticated algorithms could consider mutual friends, interests, etc.
    const char* sql = R"(
        SELECT u.id, u.username, u.profile_pic, u.bio, u.created_at
        FROM users u
        WHERE u.username != ? 
        AND u.id NOT IN (
            SELECT CASE 
                WHEN f.requester_id = current.id THEN f.addressee_id 
                ELSE f.requester_id 
            END
            FROM friends f
            JOIN users current ON current.username = ?
            WHERE (f.requester_id = current.id OR f.addressee_id = current.id)
        )
        ORDER BY u.created_at DESC
        LIMIT ?
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the friend suggestions query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);  // Exclude current user
        sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);  // User for friend relationship check
        sqlite3_bind_int(stmt, 3, limit);                                 // Limit number of suggestions
        
        // Process all suggested users found in the query
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            SearchUser user = createUserFromRow(stmt);  // Create SearchUser from database row
            suggestions.push_back(user);                // Add to suggestions list
        }
        sqlite3_finalize(stmt);  // Clean up prepared statement
    }
    
    return suggestions;  // Return list of suggested friends
}

// Public method to send a friend request from one user to another
// Parameters: requester (username sending the request), addressee (username receiving the request)
// Returns: boolean indicating success (true) or failure (false) of the operation
bool FriendSearchService::sendFriendRequest(const std::string& requester, const std::string& addressee) {
    // First, check if a friend request already exists between these users (in either direction)
    // This prevents duplicate requests and ensures data integrity
    const char* checkSql = R"(
        SELECT COUNT(*) FROM friends f
        JOIN users u1 ON f.requester_id = u1.id
        JOIN users u2 ON f.addressee_id = u2.id
        WHERE (u1.username = ? AND u2.username = ?) OR (u1.username = ? AND u2.username = ?)
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the duplicate check query
    if (sqlite3_prepare_v2(db, checkSql, -1, &stmt, NULL) == SQLITE_OK) {
        // Bind parameters to check both directions: (requester->addressee) OR (addressee->requester)
        sqlite3_bind_text(stmt, 1, requester.c_str(), -1, SQLITE_STATIC);   // u1.username = requester
        sqlite3_bind_text(stmt, 2, addressee.c_str(), -1, SQLITE_STATIC);   // u2.username = addressee
        sqlite3_bind_text(stmt, 3, addressee.c_str(), -1, SQLITE_STATIC);   // u1.username = addressee
        sqlite3_bind_text(stmt, 4, requester.c_str(), -1, SQLITE_STATIC);   // u2.username = requester
        
        // Execute query and check if any existing relationship found
        if (sqlite3_step(stmt) == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
            sqlite3_finalize(stmt);  // Clean up statement
            return false;            // Request already exists, cannot send duplicate
        }
        sqlite3_finalize(stmt);  // Clean up statement
    }
    
    // Insert new friend request with 'pending' status
    // Uses subqueries to convert usernames to user IDs for foreign key relationships
    const char* insertSql = R"(
        INSERT INTO friends (requester_id, addressee_id, status)
        VALUES (
            (SELECT id FROM users WHERE username = ?),
            (SELECT id FROM users WHERE username = ?),
            'pending'
        )
    )";
    
    // Prepare the insertion query
    if (sqlite3_prepare_v2(db, insertSql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester.c_str(), -1, SQLITE_STATIC);  // Requester username
        sqlite3_bind_text(stmt, 2, addressee.c_str(), -1, SQLITE_STATIC);  // Addressee username
        
        // Execute insertion and check if successful
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);  // Clean up statement
        return success;          // Return true if insertion successful, false otherwise
    }
    
    return false;  // Return false if query preparation failed
}

// Public method to accept a pending friend request
// Parameters: requester (username who sent the request), addressee (username accepting the request)
// Returns: boolean indicating success (true) or failure (false) of the acceptance
bool FriendSearchService::acceptFriendRequest(const std::string& requester, const std::string& addressee) {
    // SQL query to update friend request status from 'pending' to 'accepted'
    // Only updates if the request exists and is currently pending
    const char* sql = R"(
        UPDATE friends SET status = 'accepted'
        WHERE requester_id = (SELECT id FROM users WHERE username = ?)
        AND addressee_id = (SELECT id FROM users WHERE username = ?)
        AND status = 'pending'
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the acceptance update query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester.c_str(), -1, SQLITE_STATIC);  // Requester username
        sqlite3_bind_text(stmt, 2, addressee.c_str(), -1, SQLITE_STATIC);  // Addressee username
        
        // Execute update and check if successful
        // SQLITE_DONE indicates successful execution (may or may not have updated rows)
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);  // Clean up statement
        return success;          // Return true if query executed successfully
    }
    
    return false;  // Return false if query preparation failed
}

// Public method to reject/decline a pending friend request
// Parameters: requester (username who sent the request), addressee (username rejecting the request)
// Returns: boolean indicating success (true) or failure (false) of the rejection
bool FriendSearchService::rejectFriendRequest(const std::string& requester, const std::string& addressee) {
    // SQL query to update friend request status from 'pending' to 'declined'
    // Only updates if the request exists and is currently pending
    const char* sql = R"(
        UPDATE friends SET status = 'declined'
        WHERE requester_id = (SELECT id FROM users WHERE username = ?)
        AND addressee_id = (SELECT id FROM users WHERE username = ?)
        AND status = 'pending'
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the rejection update query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, requester.c_str(), -1, SQLITE_STATIC);  // Requester username
        sqlite3_bind_text(stmt, 2, addressee.c_str(), -1, SQLITE_STATIC);  // Addressee username
        
        // Execute update and check if successful
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);  // Clean up statement
        return success;          // Return true if query executed successfully
    }
    
    return false;  // Return false if query preparation failed
}

// Public method to remove/unfriend an existing friendship between two users
// Parameters: user1 (first user's username), user2 (second user's username)
// Returns: boolean indicating success (true) or failure (false) of the removal
bool FriendSearchService::removeFriend(const std::string& user1, const std::string& user2) {
    // SQL query to delete accepted friendships between two users
    // Handles bidirectional friendship: (user1->user2) OR (user2->user1)
    // Only removes friendships with 'accepted' status to preserve request history
    const char* sql = R"(
        DELETE FROM friends 
        WHERE status = 'accepted' AND (
            (requester_id = (SELECT id FROM users WHERE username = ?) AND 
             addressee_id = (SELECT id FROM users WHERE username = ?)) OR
            (requester_id = (SELECT id FROM users WHERE username = ?) AND 
             addressee_id = (SELECT id FROM users WHERE username = ?))
        )
    )";
    
    sqlite3_stmt* stmt;  // Prepared statement pointer
    // Prepare the friend removal query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        // Bind parameters to handle both directions of the friendship
        sqlite3_bind_text(stmt, 1, user1.c_str(), -1, SQLITE_STATIC);  // First direction: user1->user2
        sqlite3_bind_text(stmt, 2, user2.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, user2.c_str(), -1, SQLITE_STATIC);  // Second direction: user2->user1
        sqlite3_bind_text(stmt, 4, user1.c_str(), -1, SQLITE_STATIC);
        
        // Execute deletion and check if successful
        bool success = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);  // Clean up statement
        return success;          // Return true if deletion executed successfully
    }
    
    return false;  // Return false if query preparation failed
}

// Public method to refresh the BST search index with current database user data
// This method should be called when users are added, removed, or updated to keep search current
void FriendSearchService::refreshSearchIndex() {
    std::vector<SearchUser> users;  // Vector to hold all users from database
    
    // SQL query to retrieve all users with their basic information
    // Orders by username to improve BST balance (though not guaranteed to be optimal)
    const char* sql = "SELECT id, username, profile_pic, bio, created_at FROM users ORDER BY username";
    sqlite3_stmt* stmt;  // Prepared statement pointer
    
    // Prepare and execute the user retrieval query
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) == SQLITE_OK) {
        // Process each user row from the database
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            users.push_back(createUserFromRow(stmt));  // Convert row to SearchUser and add to vector
        }
        sqlite3_finalize(stmt);  // Clean up prepared statement
    }
    
    // Rebuild the BST with fresh user data from database
    // This clears the old tree and creates a new one with current data
    searchTree.rebuild(users);
}

// Utility method to convert a single SearchUser object to Crow JSON format
// Parameter: user (SearchUser object to convert)
// Returns: crow::json::wvalue object ready for HTTP response serialization
crow::json::wvalue FriendSearchService::userToJson(const SearchUser& user) {
    crow::json::wvalue json;  // Create empty JSON object
    
    // Convert all SearchUser fields to JSON key-value pairs
    json["id"] = user.id;                                    // User ID (integer)
    json["username"] = user.username;                        // Username (string)
    json["email"] = user.email;                              // Email address (string)
    json["profile_pic"] = user.profile_pic;                  // Profile picture URL (string)
    json["bio"] = user.bio;                                  // User biography (string)
    json["created_at"] = user.created_at;                    // Account creation timestamp (string)
    json["is_friend"] = user.is_friend;                      // Friendship status (boolean)
    json["has_pending_request"] = user.has_pending_request;  // Pending request status (boolean)
    
    return json;  // Return JSON representation of the user
}

// Utility method to convert a vector of SearchUser objects to a vector of JSON objects
// Parameter: users (vector of SearchUser objects to convert)
// Returns: vector of crow::json::wvalue objects ready for JSON array serialization
std::vector<crow::json::wvalue> FriendSearchService::usersToJson(const std::vector<SearchUser>& users) {
    std::vector<crow::json::wvalue> jsonUsers;  // Initialize empty vector for JSON objects
    
    // Convert each SearchUser to JSON and add to result vector
    for (const auto& user : users) {
        jsonUsers.push_back(userToJson(user));  // Use userToJson method for individual conversion
    }
    
    return jsonUsers;  // Return vector of JSON user objects
}
