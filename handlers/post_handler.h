// post_handler.h
#pragma once

#include <../include/crow_all.h>

class PostHandler {
public:
    static crow::response handleCreatePost(const crow::request& req);
    static crow::response handleGetAllPosts();
};