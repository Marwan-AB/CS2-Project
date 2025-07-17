#pragma once
#include "User.h"
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <string>
#include <random>

using namespace std;

class Authentication {
private:
    unordered_map<string, User*> users;
    unordered_map<string, User*> sessions;

    string hashPassword(const string& pass) {
        hash<string> hasher;
        return to_string(hasher(pass));
    }

    string generateSessionID() {
        static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        string sessionID;
        for (int i = 0; i < 16; ++i)
            sessionID += charset[rand() % (sizeof(charset) - 1)];
        return sessionID;
    }

    void loadUsersFromFile() {
        ifstream file("users.txt");
        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string uname, hash;
            getline(ss, uname, ',');
            getline(ss, hash, ',');
            users[uname] = new User(uname, hash);
        }
    }

    void saveUserToFile(const string& uname, const string& hash) {
        ofstream file("users.txt", ios::app);
        file << uname << "," << hash << "\n";
    }

public:
    Authentication() {
        loadUsersFromFile();
    }

    ~Authentication() {
        for (auto& p : users) delete p.second;
    }

    bool registerUser(const string& uname, const string& pass) {
        if (users.count(uname)) return false;
        string hash = hashPassword(pass);
        users[uname] = new User(uname, hash);
        saveUserToFile(uname, hash);
        return true;
    }

    string login(const string& uname, const string& pass) {
        if (!users.count(uname)) return "";
        string hash = hashPassword(pass);
        if (!users[uname]->verifyPassword(hash)) return "";

        string sid = generateSessionID();
        users[uname]->setSessionID(sid);
        sessions[sid] = users[uname];
        return sid;
    }

    void logout(const string& sid) {
        sessions.erase(sid);
    }

    bool isAuthenticated(const string& sid) const {
        return sessions.count(sid) > 0;
    }

    User* getUserBySession(const string& sid) {
        return sessions.count(sid) ? sessions.at(sid) : nullptr;
    }

    void clearAllSessions() {
    sessions.clear(); 
    }

    unordered_map<string, User*>& getAllUsers() {
    return users;
}
User* getUserByUsername(const string& uname) {
    return users.count(uname) ? users.at(uname) : nullptr;
}
};
