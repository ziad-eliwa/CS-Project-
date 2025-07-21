#pragma once
#ifndef FEED_SERVICE_H
#define FEED_SERVICE_H
#include <bits/stdc++.h>
#include "Post.h"
using namespace std;
class TimelineService{
    private:
    vector <Post> posts;
    public:
    vector <Post> getFeed(const string& username);
    void addPost(const Post& post);
    void deletePost(int postId);
};

#endif