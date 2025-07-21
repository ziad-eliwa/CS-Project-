// comment_handler.h
#pragma once

#include "../include/crow_all.h"
#include "../services/Comment.h"

class CommentHandler {
public:
    static crow::response handleCreateComment(const crow::request& req);
    static crow::response handleGetAllComments();
    void handleDeleteComment(const crow::request& req, crow::response& res);
};