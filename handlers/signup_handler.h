#pragma once
#include "sqlite3.h"
#include "unordered_map"
#include <iostream>
#include <crow.h>

 crow::response handle_signup(sqlite3 * db,const crow::request& req);