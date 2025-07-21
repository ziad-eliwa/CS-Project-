#pragma once
#include <sqlite3.h>
#include "../include/crow_all.h"

namespace timeline_handler {
    crow::response handle_get_timeline(sqlite3* db, const crow::request& req);
}
