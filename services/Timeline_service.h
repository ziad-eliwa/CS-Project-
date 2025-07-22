#pragma once
#ifndef FEED_SERVICE_H
#define FEED_SERVICE_H
#include <bits/stdc++.h>
#include "Timeline.h"
#include "Post.h"
using namespace std;
class TimelineService{
    private:
    // Timeline* timeline; // Remove or make pointer if needed
    public:
    TimelineService();
    TimelineService(const TimelineService&) = delete;
    TimelineService& operator=(const TimelineService&) = delete;
    void addPost(const Post& post);
    std::vector<Post> getTimelineForUser(sqlite3* db, const std::string& username);
};

#endif