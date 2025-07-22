// post_handler.h
#pragma once

#include <crow.h>

class PostHandler {
public:
    static crow::response handleCreatePost(const crow::request& req, const std::string& username);
    static crow::response handleGetAllPosts();
};