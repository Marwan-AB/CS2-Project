#include "Authentication.h"
#include <random>

string Authentication::hashPassword(const string& pass) {
    hash<string> hasher;
    return to_string(hasher(pass));
}

string Authentication::generateSessionID() {
    static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    string sessionID;
    for (int i = 0; i < 16; ++i)
        sessionID += charset[rand() % (sizeof(charset) - 1)];
    return sessionID;
}

void Authentication::loadUsersFromFile() {
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

void Authentication::saveUserToFile(const string& uname, const string& hash) {
    ofstream file("users.txt", ios::app);
    file << uname << "," << hash << "\n";
}

Authentication::Authentication() {
    loadUsersFromFile();
    for (auto& [uname, user] : users) {
        user->loadFriendDataFromFile();
    }
}

Authentication::~Authentication() {
    for (auto& p : users) delete p.second;
}

bool Authentication::registerUser(const string& uname, const string& pass) {
    if (users.count(uname)) return false;
    string hash = hashPassword(pass);
    users[uname] = new User(uname, hash);
    saveUserToFile(uname, hash);
    return true;
}

string Authentication::login(const string& uname, const string& pass) {
    if (!users.count(uname)) return "";
    string hash = hashPassword(pass);
    if (!users[uname]->verifyPassword(hash)) return "";

    string sid = generateSessionID();
    users[uname]->setSessionID(sid);
    sessions[sid] = users[uname];
    return sid;
}

void Authentication::logout(const string& sid) {
    sessions.erase(sid);
}

bool Authentication::isAuthenticated(const string& sid) const {
    return sessions.count(sid) > 0;
}

User* Authentication::getUserBySession(const string& sid) {
    return sessions.count(sid) ? sessions.at(sid) : nullptr;
}

void Authentication::clearAllSessions() {
    sessions.clear(); 
}

unordered_map<string, User*>& Authentication::getAllUsers() {
    return users;
}

User* Authentication::getUserByUsername(const string& uname) {
    return users.count(uname) ? users.at(uname) : nullptr;
}

vector<string> Authentication::searchUsersBySubstring(const string& part) {
    vector<string> result;
    for (auto& [uname, _] : users)
        if (uname.find(part) != string::npos)
            result.push_back(uname);
    return result;
}
