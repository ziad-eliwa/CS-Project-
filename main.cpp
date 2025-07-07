#include "include/crow_all.h"
#include <openssl/sha.h>
#include <sqlite3.h>
#include <bits/stdc++.h>
#include "handlers/login_handler.h"
#include "handlers/signup_handler.h"
#include "utils/db_utils.h"

int main()
{
    sqlite3 * db;

    sqlite3_open("users.db",&db);

    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT);", nullptr, nullptr, nullptr);

    crow::SimpleApp app;

    CROW_ROUTE(app, "/login").methods("POST"_method)([db](const crow::request& req){
        return handleLogin(db, req);
    });

    CROW_ROUTE(app, "/signup").methods("POST"_method)([db](const crow::request& req){
       return handleSignUp(db, req); 
    });

    app.port(18080).multithreaded().run();
    sqlite3_close(db);
}