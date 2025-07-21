#ifndef PROFILE_SERVICES_H
#define PROFILE_SERVICES_H

#pragma once

#include <string>
#include <sqlite3.h>
#include "profile.h"

struct UserProfile {
    std::string display_name;
    std::string avatar_url;
    std::string bio;
    std::string location;
    std::string website;
};

class ProfileService {
public:
    static bool getProfile(sqlite3* db, const std::string& username, UserProfile& profile) {
        Profile p;
        if (!Profile::loadByUsername(db, username, p)) return false;
        profile.display_name = p.getDisplayName();
        profile.avatar_url = p.getAvatarUrl();
        profile.bio = p.getBio();
        profile.location = p.getLocation();
        profile.website = p.getWebsite();
        return true;
    }
    static bool updateProfile(sqlite3* db, const std::string& username, const UserProfile& newProfile) {
        Profile p;
        p.setDisplayName(newProfile.display_name);
        p.setAvatarUrl(newProfile.avatar_url);
        p.setBio(newProfile.bio);
        p.setLocation(newProfile.location);
        p.setWebsite(newProfile.website);
        return Profile::updateByUsername(db, username, p);
    }
    static bool createDefaultProfile(sqlite3* db, const std::string& username) {
        return Profile::createDefault(db, username);
    }
};

#endif