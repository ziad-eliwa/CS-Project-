#ifndef LIKE_SERVICE_H
#define LIKE_SERVICE_H

#include <sqlite3.h>
#include <string>

class LikeService {
public:
    static bool toggleLike(sqlite3* db, int post_id, const std::string& username);
    static bool isPostLikedByUser(sqlite3* db, int post_id, const std::string& username);
};

#endif // LIKE_SERVICE_H
