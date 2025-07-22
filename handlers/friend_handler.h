#pragma once

#include <sqlite3.h>
#include <crow.h>

crow::response handleSearchUsers(sqlite3* db, const crow::request& req);

crow::response handleGetFriends(sqlite3* db, const crow::request& req);

crow::response handleSendFriendRequest(sqlite3* db, const crow::request& req);

crow::response handleRespondToFriendRequest(sqlite3* db, const crow::request& req);

crow::response handleRemoveFriend(sqlite3* db, const crow::request& req);

crow::response handleGetFriendSuggestions(sqlite3* db, const crow::request& req);

crow::response handleGetFriendRequests(sqlite3* db, const crow::request& req);