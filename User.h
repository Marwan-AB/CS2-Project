
#pragma once
#include <string>
#include <vector>
#include "Post.h"

using namespace std;

class User {
private:
    string Username;
    string HashedPassword;
    vector<string> Notifications;
    string sessionID;
    vector<Post> posts;
    //others will be implemnted later
public:
    User(const string& uname, const string& hash): Username(uname), HashedPassword(hash) {}

    string getUsername() const { return Username; }

    bool verifyPassword(const string& hashedAttempt) const {
        return HashedPassword == hashedAttempt;
    }

    void setSessionID(const string& sid) {
        sessionID = sid;
    }

    string getSessionID() const {
        return sessionID;
    }

    void addPost(const string& content) {
        Post p(content, Username);
        posts.push_back(p);
        ofstream file("posts_" + Username + ".txt", ios::app);
        file << p.getTimestamp() << "|" << p.getContent() << "\n";
    }

    void loadPostsFromFile() {
    ifstream file("posts_" + Username + ".txt");
    string line;
    while (getline(file, line)) {
        size_t sep = line.find('|');
        if (sep == string::npos) continue;
        time_t ts = stoll(line.substr(0, sep));
        string content = line.substr(sep + 1);
        posts.emplace_back(content, Username, ts);
    }
    }

    const vector<Post>& getPosts() const {
        return posts;
    }
    // rest of functions will be implemented later
};

