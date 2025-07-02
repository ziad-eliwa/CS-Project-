#include <iostream>
#include <string>
#include <vector>

using namespace std;

class User{
    string username;
    string name;
    string password;
    string email; 
    int phone; 
    
    public : 

    void signUp(string, string, string, string, int);
    void login(string, string);
    void logout();

    // Setters

    void setUsername(string);
    void setName(string);
    void setPassword(string);
    void setEmail(string);
    void setPhone(int);

    // Getters

    string getUsername() const;
    string getName() const;
    string getPassword() const;
    string getEmail() const;
    int getPhone() const;
    
};

class Profile : protected User{

    string profilePicture; // stores the link of the picture
    string bio; 
    vector <string> friends;
    vector <Post> posts; // contains all the posts of the users
    string birth;

    public : 

    void addFriend(string); 
    void removeFriend(string);
    void addPost(Post);
    void removePost(Post);

    // Setters

    void setProfilePicture(string);
    void setBio(string);
    void setBirth(string);

    // Getters

    string getProfilePicture() const; // is it needed ? i dont think so
    string getBirth() const;

};


class Post{

    string caption; 
    string image; // stores the link of the image and is optional

    public : 
    string postURL(); // returns the link of the post

    // Setters 

    void setCaption(string);
    void setImage(string);
};