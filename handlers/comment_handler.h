// comment_handler.h
#pragma once

#include "../include/crow_all.h"
#include "../services/Comment.h"
#include <sqlite3.h>

crow::response handle_create_comment(sqlite3* db, const crow::request& req);
crow::response handle_get_comments(sqlite3* db, const crow::request& req);