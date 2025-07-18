
#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include "Post.h"
#include "AVLTree.h"

using namespace std;

class User {
private:
    string Username;
    string HashedPassword;
    vector<string> Notifications;
    string sessionID;
    vector<Post> posts;
    AVLTree<string> friends;
    AVLTree<string> sentRequests;
    AVLTree<string> receivedRequests;

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

    void addPost(const string& content, const unordered_map<string, User*>& allUsers) {
        Post p(content, Username);
        posts.push_back(p);
        ofstream file("posts_" + Username + ".txt", ios::app);
        file << p.getTimestamp() << "|" << p.getContent() << "\n";
        for (const string& friendName : friends.inOrderTraversal()) {
            auto it = allUsers.find(friendName);
            if (it != allUsers.end() && it->second) {
                it->second->addNotification(Username + " added a new post.");
            }
        }
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

    void sendFriendRequest(User* other) {
        if (!other || other->Username == Username || friends.contains(other->Username)) return;
        if (sentRequests.contains(other->Username) || receivedRequests.contains(other->Username)) return;
        sentRequests.insert(other->Username);
        other->receivedRequests.insert(Username);
        this->saveFriendDataToFile();
        other->saveFriendDataToFile();
        other->addNotification("You have a new friend request from " + Username);
    }

    void acceptFriendRequest(User* other) {
        if (!other || !receivedRequests.contains(other->Username)) return;
        receivedRequests.remove(other->Username);
        other->sentRequests.remove(Username);
        friends.insert(other->Username);
        other->friends.insert(Username);
        this->saveFriendDataToFile();
        other->saveFriendDataToFile();
    }

    void rejectFriendRequest(User* other) {
        if (!other || !receivedRequests.contains(other->Username)) return;
        receivedRequests.remove(other->Username);
        other->sentRequests.remove(Username);
        this->saveFriendDataToFile();
        other->saveFriendDataToFile();
    }

    void cancelFriendRequest(User* other) {
        if (!other || !sentRequests.contains(other->Username)) return;
        sentRequests.remove(other->Username);
        other->receivedRequests.remove(Username);
        this->saveFriendDataToFile();
        other->saveFriendDataToFile();
    }

    void removeFriend(User* other) {
    if (!other || !friends.contains(other->getUsername())) return;
    friends.remove(other->getUsername());
    other->friends.remove(Username);
    saveFriendDataToFile();
    other->saveFriendDataToFile();
}

    vector<string> getFriendList() const {
        return friends.inOrderTraversal();
    }

    vector<string> getSentRequests() const {
        return sentRequests.inOrderTraversal();
    }

    vector<string> getReceivedRequests() const {
        return receivedRequests.inOrderTraversal();
    }

    bool isFriendWith(const string& uname) const {
        return friends.contains(uname);
    }

    bool hasSentRequestTo(const string& uname) const {
        return sentRequests.contains(uname);
    }

    bool hasReceivedRequestFrom(const string& uname) const {
        return receivedRequests.contains(uname);
    }

    int mutualFriendCount(User* other) const {
        if (!other) return 0;
        vector<string> myFriends = getFriendList();
        int count = 0;
        for (const string& f : myFriends)
            if (other->isFriendWith(f)) ++count;
        return count;
    }

    vector<string> suggestFriends(const unordered_map<string, User*>& allUsers) const {
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

    void saveFriendDataToFile() {
        ofstream f("friends_" + Username + ".txt");
        for (const string& name : friends.inOrderTraversal())
            f << name << "\n";

        ofstream r("requests_" + Username + ".txt");
        for (const string& name : receivedRequests.inOrderTraversal())
            r << "r:" << name << "\n";
        for (const string& name : sentRequests.inOrderTraversal())
            r << "s:" << name << "\n";
    }

    void loadFriendDataFromFile() {
        ifstream f("friends_" + Username + ".txt");
        string name;
        while (getline(f, name))
            friends.insert(name);

        ifstream r("requests_" + Username + ".txt");
        string line;
        while (getline(r, line)) {
            if (line.size() < 3) continue;
            if (line[0] == 'r')
                receivedRequests.insert(line.substr(2));
            else if (line[0] == 's')
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

    void addNotification(const string& msg) {
    Notifications.push_back(msg);
    }

    vector<string> getNotifications() const {
        return Notifications;
    }

    void clearNotifications() {
        Notifications.clear();
    }

};


