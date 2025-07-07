#pragma once
#include <sqlite3.h>
#include <string>

bool checkCredentials(sqlite3* db, const std::string& username, const std::string& password);
bool userExists(sqlite3* db, const std::string& username);
bool registerUser(sqlite3* db, const std::string& username, const std::string& password);
