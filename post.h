#pragma once
#include <string>
#include <ctime>

using namespace std;

class Post {
private:
    string content;
    string author;
    time_t timestamp;

public:
    Post(const string& text, const string& user): content(text), author(user), timestamp(time(nullptr)) {}
    Post(const string& text, const string& user, time_t time): content(text), author(user), timestamp(time) {}

    string getContent() const { return content; }
    string getAuthor() const { return author; }
    time_t getTimestamp() const { return timestamp; }
    string getUsername() const {return author;}

};
