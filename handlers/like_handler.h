#ifndef LIKE_HANDLER_H
#define LIKE_HANDLER_H

#include <crow.h>
#include <sqlite3.h>
#include <string>

crow::response handleToggleLike(sqlite3* db, const crow::request& req, const std::string& username);
crow::response handleGetLikeStatus(sqlite3* db, const crow::request& req, const std::string& username);

#endif // LIKE_HANDLER_H