
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
    if (!other || other->Username == Username || friends.contains(other->Username))
        return;
    if (sentRequests.count(other->Username) || receivedRequests.count(other->Username))
        return;
    sentRequests.insert(other->Username);
    other->receivedRequests.insert(Username);
    }

    void User::acceptFriendRequest(User* other) {
    if (!other || !receivedRequests.count(other->Username)) return;
    receivedRequests.erase(other->Username);
    other->sentRequests.erase(Username);

    friends.insert(other->Username);
    other->friends.insert(Username);
    }

    void User::rejectFriendRequest(User* other) {
    if (!other || !receivedRequests.count(other->Username)) return;
    receivedRequests.erase(other->Username);
    other->sentRequests.erase(Username);
    }

    void User::cancelFriendRequest(User* other) {
    if (!other || !sentRequests.count(other->Username)) return;
    sentRequests.erase(other->Username);
    other->receivedRequests.erase(Username);
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

    // rest of functions will be implemented later
};

