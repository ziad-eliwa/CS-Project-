#pragma once

#include "../include/crow_all.h"
#include <sqlite3.h>

crow::response handleLogin(sqlite3* db, const crow::request& req);