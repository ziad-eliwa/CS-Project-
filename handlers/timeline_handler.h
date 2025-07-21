#pragma once
#include "../include/crow_all.h"
#include <sqlite3.h>

crow::response get_timeline(sqlite3* db, const crow::request& req);
crow::response add_post(sqlite3* db, const crow::request& req);
crow::response delete_post(sqlite3* db, const crow::request& req, int postId);
