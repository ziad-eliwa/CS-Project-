#include "sqlite3.h"
#include "unordered_map"
#include <iostream>
#include "../include/crow_all.h"

 crow::response handle_login(sqlite3 * db, const crow::request& req);
 std::unordered_map<std::string, std::string> parseform_data(const std::string& body);
