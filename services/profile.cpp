#include "profile.h"
#include <sqlite3.h>

Profile::Profile() : user_id(-1), display_name(""), avatar_url(""), bio(""), location(""), website("") {}

Profile::Profile(int user_id, const std::string& display_name, const std::string& avatar_url, const std::string& bio, const std::string& location, const std::string& website)
    : user_id(user_id), display_name(display_name), avatar_url(avatar_url), bio(bio), location(location), website(website) {}

int Profile::getUserId() const { return user_id; }
std::string Profile::getDisplayName() const { return display_name; }
std::string Profile::getAvatarUrl() const { return avatar_url; }
std::string Profile::getBio() const { return bio; }
std::string Profile::getLocation() const { return location; }
std::string Profile::getWebsite() const { return website; }

void Profile::setDisplayName(const std::string& v) { display_name = v; }
void Profile::setAvatarUrl(const std::string& v) { avatar_url = v; }
void Profile::setBio(const std::string& v) { bio = v; }
void Profile::setLocation(const std::string& v) { location = v; }
void Profile::setWebsite(const std::string& v) { website = v; }

bool Profile::loadByUsername(sqlite3* db, const std::string& username, Profile& profile) {
    std::string query = "SELECT profile.user_id, profile.display_name, profile.avatar_url, profile.bio, profile.location, profile.website "
                        "FROM profile JOIN users ON profile.user_id = users.id WHERE users.username = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        profile.user_id = sqlite3_column_int(stmt, 0);
        profile.display_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        profile.avatar_url   = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        profile.bio         = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        profile.location    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        profile.website     = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

bool Profile::updateByUsername(sqlite3* db, const std::string& username, const Profile& profile) {
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
    std::string query = "UPDATE profile SET display_name = ?, avatar_url = ?, bio = ?, location = ?, website = ? WHERE user_id = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, profile.display_name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, profile.avatar_url.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, profile.bio.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, profile.location.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, profile.website.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, user_id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool Profile::createDefault(sqlite3* db, const std::string& username) {
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
    if (sqlite3_prepare_v2(db, insert_query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, user_id);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
