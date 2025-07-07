#include "db_utils.h"
#include "hash_utils.h"

bool checkCredentials(sqlite3* db, const std::string& username, const std::string& password){
    std::string hashedPassword = hashed(password);
    std::string query = "SELECT * FROM users WHERE username = ? AND password = ?;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);

    bool found = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return found;
}

bool userExists(sqlite3* db, const std::string& username){
    std::string query = "SELECT 1 FROM users WHERE username = ?;";
    
    sqlite3_stmt * stmt;

    sqlite3_prepare_v2(db,query.c_str(),-1,&stmt,nullptr);

    sqlite3_bind_text(stmt,1,username.c_str(),-1,SQLITE_STATIC);

    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;    
}
bool registerUser(sqlite3* db, const std::string& username, const std::string& password){
    std::string hashedPassword = hashed(password);
    std::string query = "INSERT INTO users (username,password) VALUES(?,?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return false;

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}
