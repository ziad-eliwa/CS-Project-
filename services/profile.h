#ifndef PROFILE_H
#define PROFILE_H
#include <string>
#include <sqlite3.h>

class Profile {
private:
    int user_id;
    std::string display_name;
    std::string avatar_url;
    std::string bio;
    std::string location;
    std::string website;
public:
    Profile();
    Profile(int user_id, const std::string& display_name, const std::string& avatar_url, const std::string& bio, const std::string& location, const std::string& website);
    // Getters
    int getUserId() const;
    std::string getDisplayName() const;
    std::string getAvatarUrl() const;
    std::string getBio() const;
    std::string getLocation() const;
    std::string getWebsite() const;
    // Setters
    void setDisplayName(const std::string&);
    void setAvatarUrl(const std::string&);
    void setBio(const std::string&);
    void setLocation(const std::string&);
    void setWebsite(const std::string&);
    // DB operations
    static bool loadByUsername(sqlite3* db, const std::string& username, Profile& profile);
    static bool updateByUsername(sqlite3* db, const std::string& username, const Profile& profile);
    static bool createDefault(sqlite3* db, const std::string& username);
};
#endif