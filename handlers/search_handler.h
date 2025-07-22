#pragma once
#include <crow.h>
#include <sqlite3.h>
#include <nlohmann/json.hpp>
#include <string>

namespace search_handler {
    crow::response handle_search_posts(sqlite3* db, const crow::request& req);
}