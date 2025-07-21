#include "User.h"
#include <ctime>
#include <sqlite3.h>
#include <iostream>

User::User() {}

User::User(const std::string& username, const std::string& password_hash, const std::string& profile_pic, const std::string& bio, const time_t& created_at)
    : username(username), password_hash(password_hash), profile_picURL(profile_pic), bio(bio), created_at(created_at) {}

void User::setUsername(const std::string& uname) {
    username = uname;
}
void User::setPasswordHash(const std::string& hash) {
    password_hash = hash;
}
void User::setProfilePic(const std::string& pic) {
    profile_picURL = pic;
}
void User::setBio(const std::string& b) {
    bio = b;
}
void User::setCreatedAt(const time_t& date) {
    created_at = date;
}

std::string User::getUsername() const {
    return username;
}
std::string User::getPasswordHash() const {
    return password_hash;
}
std::string User::getProfilePicURL() const {
    return profile_picURL;
}
std::string User::getBio() const {
    return bio;
}
time_t User::getCreatedAt() const {
    return created_at;
}
void User::setUserID(std::string& id)
{   
    user_id = id;
}
std::string User::getUserID() const{
    return user_id;
}

bool User::createInDB(sqlite3* db, const std::string& username, const std::string& password) {
    std::string hashedPassword = password; // TODO: hash if needed
    std::string query = "INSERT INTO users (username, password_hash) VALUES (?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

User User::loadByID(sqlite3* db, const std::string& user_id) {
    std::string query = "SELECT id, username, password_hash, profile_pic, bio, created_at FROM users WHERE id = ?;";
    sqlite3_stmt* stmt;
    User user;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string pic = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::string bio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            time_t created = static_cast<time_t>(sqlite3_column_int64(stmt, 5));
            user = User(uname, pass, pic, bio, created);
            user.setUserID(id);
        }
        sqlite3_finalize(stmt);
    }
    return user;
}

User User::loadByUsername(sqlite3* db, const std::string& username) {
    std::string query = "SELECT id, username, password_hash, profile_pic, bio, created_at FROM users WHERE username = ?;";
    sqlite3_stmt* stmt;
    User user;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            std::string pic = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            std::string bio = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            time_t created = static_cast<time_t>(sqlite3_column_int64(stmt, 5));
            user = User(uname, pass, pic, bio, created);
            user.setUserID(id);
        }
        sqlite3_finalize(stmt);
    }
    return user;
}

bool User::updateInDB(sqlite3* db) const {
    std::string query = "UPDATE users SET username = ?, password_hash = ?, profile_pic = ?, bio = ? WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password_hash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, profile_picURL.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, bio.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, user_id.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool User::deleteInDB(sqlite3* db) const {
    std::string query = "DELETE FROM users WHERE id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;
    sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
