
#pragma once
#include <string>
#include <vector>
#include "Post.h"
#include "AVLTree.h"
#include <unordered_set>

using namespace std;

class User {
private:
    string Username;
    string HashedPassword;
    vector<string> Notifications;
    string sessionID;
    vector<Post> posts;
    AVLTree<string> friends;
    unordered_set<string> sentRequests;
    unordered_set<string> receivedRequests;

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

    void User::sendFriendRequest(User* other) {
    if (!other || other->Username == Username || friends.contains(other->Username))return;
    if (sentRequests.count(other->Username) || receivedRequests.count(other->Username)) return;
    sentRequests.insert(other->Username);
    other->receivedRequests.insert(Username);
    this->saveFriendDataToFile();
    other->saveFriendDataToFile();
    }

    void User::acceptFriendRequest(User* other) {
    if (!other || !receivedRequests.count(other->Username)) return;
    receivedRequests.erase(other->Username);
    other->sentRequests.erase(Username);

    friends.insert(other->Username);
    other->friends.insert(Username);
    this->saveFriendDataToFile();
    other->saveFriendDataToFile();

    }

    void User::rejectFriendRequest(User* other) {
    if (!other || !receivedRequests.count(other->Username)) return;
    receivedRequests.erase(other->Username);
    other->sentRequests.erase(Username);
    this->saveFriendDataToFile();
    other->saveFriendDataToFile();
    }

    void User::cancelFriendRequest(User* other) {
    if (!other || !sentRequests.count(other->Username)) return;
    sentRequests.erase(other->Username);
    other->receivedRequests.erase(Username);
    this->saveFriendDataToFile();
    other->saveFriendDataToFile();
}

    void User::removeFriend(User* other) {
    if (!other || !friends.contains(other->Username)) return;
    friends.remove(other->Username);
    other->friends.remove(Username);
}
    vector<string> User::getFriendList() const {
    return friends.inOrderTraversal();
}

vector<string> User::getSentRequests() const {
    return vector<string>(sentRequests.begin(), sentRequests.end());
}

vector<string> User::getReceivedRequests() const {
    return vector<string>(receivedRequests.begin(), receivedRequests.end());
}

bool User::isFriendWith(const string& uname) const {
    return friends.contains(uname);
}

bool User::hasSentRequestTo(const string& uname) const {
    return sentRequests.count(uname);
}

bool User::hasReceivedRequestFrom(const string& uname) const {
    return receivedRequests.count(uname);
}
int User::mutualFriendCount(User* other) const {
    if (!other) return 0;
    vector<string> myFriends = getFriendList();
    int count = 0;
    for (const string& f : myFriends)
        if (other->isFriendWith(f)) ++count;
    return count;
}
vector<string> User::suggestFriends(const unordered_map<string, User*>& allUsers) const {
    vector<pair<string, int>> scored;
    for (auto& [uname, u] : allUsers) {
        if (uname == Username) continue;
        if (isFriendWith(uname)) continue;
        if (hasSentRequestTo(uname) || hasReceivedRequestFrom(uname)) continue;
        int mutual = mutualFriendCount(u);
        if (mutual > 0) scored.emplace_back(uname, mutual);
    }
    sort(scored.begin(), scored.end(), [](auto& a, auto& b) {
        return a.second > b.second;
    });

    vector<string> suggestions;
    for (auto& [name, _] : scored) suggestions.push_back(name);
    return suggestions;
}

void User::saveFriendDataToFile() {
    ofstream f("friends_" + Username + ".txt");
    for (const string& name : friends.inOrderTraversal())
        f << name << "\n";

    ofstream r("requests_" + Username + ".txt");
    for (const string& name : receivedRequests)
        r << "r:" << name << "\n";
    for (const string& name : sentRequests)
        r << "s:" << name << "\n";
}

void User::loadFriendDataFromFile() {
    ifstream f("friends_" + Username + ".txt");
    string name;
    while (getline(f, name))
        friends.insert(name);

    ifstream r("requests_" + Username + ".txt");
    string line;
    while (getline(r, line)) {
        if (line.size() < 3) continue; // safety check
        if (line[0] == 'R')
            receivedRequests.insert(line.substr(2));
        else if (line[0] == 'S')
            sentRequests.insert(line.substr(2));
    }
}

vector<string> searchUsersByPrefix(const string& query, const unordered_map<string, User*>& allUsers) const {
    vector<string> result;
    for (const auto& [uname, user] : allUsers) {
        if (uname != Username &&
            uname.size() >= query.size() &&
            uname.substr(0, query.size()) == query &&
            !isFriendWith(uname) &&
            !hasSentRequestTo(uname) &&
            !hasReceivedRequestFrom(uname)) {
            result.push_back(uname);
        }
    }
    return result;
}


    // rest of functions will be implemented later
};

