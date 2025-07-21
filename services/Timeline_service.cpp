#include "Timeline_service.h"
#include "Timeline.h"
#include "Post.h"
#include <algorithm>
#include "User.h"

TimelineService::TimelineService() {}

TimelineService::TimelineService(Timeline& t) : timeline(t) {}

TimelineService::TimelineService(const TimelineService& other) : timeline(other.timeline) {}

std::vector<Post> TimelineService::getTimelineForUser(sqlite3* db, const std::string& username) {
    User user = User::loadByUsername(db, username);
    std::string user_id = user.getUserID();
    if (user_id.empty()) {
        return {}; 
    }
    return Timeline::fetchTimeline(db, user_id);
}

void TimelineService::addPost(const Post& post) {
    timeline.addPost(post);
}

void TimelineService::deletePost(int postId) {
    Post dummy;
    dummy.setId(postId); 
    timeline.deletePost(dummy);
}