
#pragma once
#include <string>
#include <vector>

using namespace std;

class User {
private:
    string Username;
    string HashedPassword;
    vector<string> Notifications;
    string sessionID;

public:
    User(const string& uname, const string& hash)
        : Username(uname), HashedPassword(hash) {}

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
};

