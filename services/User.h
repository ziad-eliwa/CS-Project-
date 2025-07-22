#ifndef USER_H
#define USER_H

#include <bits/stdc++.h>
#include <ctime>
#include <string>
#include "profile.h"
#include "FriendManager.h"
#include <sqlite3.h>

class User{
    private:
        std::string user_id;
        std::string username;
        std::string password_hash;
        std::string profile_picURL;
        std::string bio;
        time_t created_at;
    public:
        // Constructors
        User();
        User(const std::string& username, const std::string& password_hash, const std::string& profile_pic, const std::string& bio, const time_t& created_at);
        void setUsername(const std::string& uname);
        void setPasswordHash(const std::string& hash);
        void setProfilePic(const std::string& pic);
        void setBio(const std::string& b);
        void setCreatedAt(const time_t& date);
        void setUserID(std::string&);

        std::string getUsername() const;
        std::string getUserID() const;
        std::string getPasswordHash() const;
        std::string getProfilePicURL() const;
        std::string getBio() const;
        time_t getCreatedAt() const;
        
        static User loadByID(sqlite3* db, const std::string& user_id);
        static User loadByUsername(sqlite3* db, const std::string& username);
        static bool createInDB(sqlite3* db, const std::string& username, const std::string& password);
        bool updateInDB(sqlite3* db) const;
        bool deleteInDB(sqlite3* db) const;
};

#endif