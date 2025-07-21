#ifndef TIMELINE_H
#define TIMELINE_H

#include "Post.h"
#include "User.h"
#include <bits/stdc++.h>
#include <sqlite3.h>
using namespace std;
class Timeline {
private:
  User user;
  vector<Post> posts;

public:
  Timeline();
  Timeline(vector<Post> &);
  ~Timeline();
  std::vector<Post> getPosts() const;
  bool addPost(const Post &);
  std::string getUser_ID();
  void setUserID(std::string &);
  std::vector<Post> generateTimeline(const std::vector<Post>& userPosts, const std::vector<Post>& friendsPosts);
  static std::vector<Post> fetchTimeline(sqlite3* db, const std::string& user_id);
};

#endif