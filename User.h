#pragma once

#include <string>
#include <vector>
#include <unordered_map>
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
    User(const string& uname, const string& hash);

    string getUsername() const;
    bool verifyPassword(const string& hashedAttempt) const;
    void setSessionID(const string& sid);
    string getSessionID() const;

    void addPost(const string& content, const unordered_map<string, User*>& allUsers);
    void loadPostsFromFile();
    const vector<Post>& getPosts() const;

    void sendFriendRequest(User* other);
    void acceptFriendRequest(User* other);
    void rejectFriendRequest(User* other);
    void cancelFriendRequest(User* other);
    void removeFriend(User* other);

    vector<string> getFriendList() const;
    vector<string> getSentRequests() const;
    vector<string> getReceivedRequests() const;
    bool isFriendWith(const string& uname) const;
    bool hasSentRequestTo(const string& uname) const;
    bool hasReceivedRequestFrom(const string& uname) const;
    int mutualFriendCount(User* other) const;
    vector<string> suggestFriends(const unordered_map<string, User*>& allUsers) const;
    void saveFriendDataToFile();
    void loadFriendDataFromFile();
    vector<string> searchUsersByPrefix(const string& query) const;

    vector<Post> getTimelinePosts(const unordered_map<string, User*>& allUsers);

    void addNotification(const string& msg);
    vector<string> getNotifications() const;
    void clearNotifications();
};



