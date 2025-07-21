#include "profile_services.h"
#include <sqlite3.h>

bool ProfileService::getProfile(sqlite3* db, const std::string& username, UserProfile& profile) {
    std::string query =
        "SELECT profile.display_name, profile.avatar_url, profile.bio, profile.location, profile.website "
        "FROM profile JOIN users ON profile.user_id = users.id "
        "WHERE users.username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    bool success = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        profile.display_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        profile.avatar_url   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        profile.bio         = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        profile.location    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        profile.website     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        success = true;
    }
    sqlite3_finalize(stmt);
    return success;
}

bool ProfileService::updateProfile(sqlite3* db, const std::string& username, const UserProfile& newProfile) {
 
    std::string user_id_query = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* id_stmt;
    int user_id = -1;
    if (sqlite3_prepare_v2(db, user_id_query.c_str(), -1, &id_stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(id_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(id_stmt) == SQLITE_ROW) {
            user_id = sqlite3_column_int(id_stmt, 0);
        }
    }
    sqlite3_finalize(id_stmt);
    if (user_id == -1) return false;

    std::string query =
        "UPDATE profile SET display_name = ?, avatar_url = ?, bio = ?, location = ?, website = ? "
        "WHERE user_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_text(stmt, 1, newProfile.display_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, newProfile.avatar_url.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, newProfile.bio.c_str(),       -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, newProfile.location.c_str(),  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, newProfile.website.c_str(),   -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, user_id);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool ProfileService::createDefaultProfile(sqlite3* db, const std::string& username) {
    std::string user_id_query = "SELECT id FROM users WHERE username = ?;";
    sqlite3_stmt* id_stmt;
    int user_id = -1;
    if (sqlite3_prepare_v2(db, user_id_query.c_str(), -1, &id_stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(id_stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(id_stmt) == SQLITE_ROW) {
            user_id = sqlite3_column_int(id_stmt, 0);
        }
    }
    sqlite3_finalize(id_stmt);
    if (user_id == -1) return false;

    std::string insert_query = "INSERT INTO profile (user_id, display_name, avatar_url, bio, location, website) VALUES (?, '', '', '', '', '');";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, insert_query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    sqlite3_bind_int(stmt, 1, user_id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
