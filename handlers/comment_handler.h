// comment_handler.h
#pragma once

#include <crow.h>
#include "../services/Comment.h"
#include <sqlite3.h>

crow::response handle_create_comment(sqlite3* db, const crow::request& req, const std::string& username);
crow::response handle_get_comments(sqlite3* db, const crow::request& req);