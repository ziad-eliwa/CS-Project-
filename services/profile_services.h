#ifndef PROFILE_SERVICES_H
#define PROFILE_SERVICES_H

#pragma once

#include <string>
#include <sqlite3.h>

struct UserProfile {
    std::string display_name;
    std::string avatar_url;
    std::string bio;
    std::string location;
    std::string website;
};

class ProfileService {
public:

    static bool getProfile(sqlite3* db, const std::string& username, UserProfile& profile);


    static bool updateProfile(sqlite3* db, const std::string& username, const UserProfile& newProfile);


    static bool createDefaultProfile(sqlite3* db, const std::string& username);
};





#endif