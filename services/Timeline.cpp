#include "Post.h"
#include "Timeline.h"
#include <bits/stdc++.h>
#include <sqlite3.h>
using namespace std;

Timeline::Timeline()
{
    posts = vector <Post>();
};

Timeline::Timeline(vector <Post>& postd) : posts(postd)
{
    sort(posts.begin(), posts.end(), [](const Post& a, const Post& b) {
        return a.getCreationAt() > b.getCreationAt(); 
    });
}

Timeline::~Timeline()
{
    posts.clear();
}

std::vector<Post> Timeline::getPosts() const {
    return posts;
}

bool Timeline::addPost(const Post& postd)
{
    posts.push_back(postd);
}
bool Timeline::deletePost(const Post& postd)
{
    auto it = std::remove_if(posts.begin(), posts.end(), [&](const Post& p) {
        return p.getId() == postd.getId();
    });
    if (it != posts.end()) {
        posts.erase(it, posts.end());
        return true;
    }
    return false;
}

std::string Timeline::getUser_ID()
{
    return user.getUserID();
}
void Timeline::setUserID(std::string& user)
{
    this->user.setUserID(user);
}

std::vector<Post> Timeline::generateTimeline(const std::vector<Post>& userPosts, const std::vector<Post>& friendsPosts) {
    std::vector<Post> timeline = userPosts;
    timeline.insert(timeline.end(), friendsPosts.begin(), friendsPosts.end());
    std::sort(timeline.begin(), timeline.end(), [](const Post& a, const Post& b) {
        return a.getCreationAt() > b.getCreationAt();
    });
    return timeline;
}

std::vector<Post> Timeline::fetchTimeline(sqlite3* db, const std::string& user_id) {
    std::vector<Post> timelinePosts;
    std::string userPostsQuery = "SELECT id, content, user_id, created_at FROM posts WHERE user_id = ?";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, userPostsQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string author_id = std::to_string(sqlite3_column_int(stmt, 2));
            time_t created_at = static_cast<time_t>(sqlite3_column_int64(stmt, 3));
            timelinePosts.emplace_back(id, content, author_id, created_at);
        }
        sqlite3_finalize(stmt);
    }
    std::string friendsPostsQuery = R"(
        SELECT p.id, p.content, p.user_id, p.created_at
        FROM posts p
        JOIN friends f ON (f.requester_id = ? AND f.addressee_id = p.user_id AND f.status = 'accepted')
                        OR (f.addressee_id = ? AND f.requester_id = p.user_id AND f.status = 'accepted')
    )";
    if (sqlite3_prepare_v2(db, friendsPostsQuery.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, user_id.c_str(), -1, SQLITE_STATIC);
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            std::string content = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            std::string author_id = std::to_string(sqlite3_column_int(stmt, 2));
            time_t created_at = static_cast<time_t>(sqlite3_column_int64(stmt, 3));
            timelinePosts.emplace_back(id, content, author_id, created_at);
        }
        sqlite3_finalize(stmt);
    }
    std::sort(timelinePosts.begin(), timelinePosts.end(), [](const Post& a, const Post& b) {
        return a.getCreationAt() > b.getCreationAt();
    });
    return timelinePosts;
}