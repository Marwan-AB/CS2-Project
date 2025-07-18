#pragma once

#include "User.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Authentication {
private:
    unordered_map<string, User*> users;
    unordered_map<string, User*> sessions;

    string hashPassword(const string& pass);
    string generateSessionID();
    void loadUsersFromFile();
    void saveUserToFile(const string& uname, const string& hash);

public:
    Authentication();
    ~Authentication();

    bool registerUser(const string& uname, const string& pass);
    string login(const string& uname, const string& pass);
    void logout(const string& sid);
    bool isAuthenticated(const string& sid) const;
    User* getUserBySession(const string& sid);
    void clearAllSessions();
    unordered_map<string, User*>& getAllUsers();
    User* getUserByUsername(const string& uname);
    vector<string> searchUsersBySubstring(const string& part);
};

