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
        return a.getcreated_at() > b.getcreated_at(); 
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
    return true;
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
        return a.getcreated_at() > b.getcreated_at();
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
    std::sort(timelinePosts.begin(), timelinePosts.end(), [](const Post& a, const Post& b) {
        return a.getcreated_at() > b.getcreated_at();
    });
    return timelinePosts;
}