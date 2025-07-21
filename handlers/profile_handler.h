#pragma once
#include "../include/crow_all.h"
#include <sqlite3.h>

crow::response handle_get_profile(sqlite3* db, const crow::request& req);
crow::response handle_update_profile(sqlite3* db, const crow::request& req);
