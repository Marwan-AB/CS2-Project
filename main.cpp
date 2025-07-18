#include <crow.h>
#include "Authentication.h"
#include "User.h"
#include <fstream>
#include <sstream>
#include <filesystem>
namespace fs = std::filesystem;
using namespace std;
int main() {
    crow::SimpleApp app;
    Authentication auth;
    CROW_ROUTE(app, "/")([]() {
    ifstream file("UI/auth.html");
    if (!file.is_open()) return crow::response(500, "HTML file not found");

    stringstream buffer;
    buffer << file.rdbuf();
    return crow::response{buffer.str()};
});
  CROW_ROUTE(app, "/home")
([] {
    ifstream file("UI/home.html");
    if (!file.is_open()) {
        cerr << "Failed to open UI/home.html\n";
        return crow::response(500, "Home page not found");
    }

    stringstream buffer;
    buffer << file.rdbuf();

    crow::response res;
    res.code = 200;
    res.set_header("Content-Type", "text/html");
    res.body = buffer.str();
    return res;
});
    CROW_ROUTE(app, "/signup").methods("POST"_method)([&auth](const crow::request& req) {
        auto body = crow::json::load(req.body);
        if (!body) return crow::response(400, "Invalid JSON");

        string uname = body["username"].s();
        string pwd = body["password"].s();

        if (auth.registerUser(uname, pwd))
            return crow::response(200, "Signup successful");
        else
            return crow::response(409, "Username already exists");
    });

    CROW_ROUTE(app, "/login").methods("POST"_method)([&auth](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400, "Invalid JSON");

    string uname = body["username"].s();
    string pwd = body["password"].s();

    string sid = auth.login(uname, pwd);
    if (!sid.empty()) {
        
        User* user = auth.getUserBySession(sid);
        if (user) user->loadPostsFromFile();

        crow::json::wvalue res;
        res["status"] = "ok";
        res["sessionID"] = sid;
        return crow::response{res};
    } else {
        return crow::response(401, "Invalid credentials");
    }
});

    CROW_ROUTE(app, "/logout").methods("POST"_method)([&auth](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body || !body.has("sessionID"))
        return crow::response(400, "Missing sessionID");

    string sid = body["sessionID"].s();
    auth.logout(sid);
    return crow::response(200);
});

CROW_ROUTE(app, "/newpost")
([] {
    ifstream file("UI/newpost.html");
    if (!file.is_open())
        return crow::response(500, "newpost.html not found");

    stringstream buffer;
    buffer << file.rdbuf();

    crow::response res;
    res.code = 200;
    res.set_header("Content-Type", "text/html");
    res.body = buffer.str();
    return res;
});

CROW_ROUTE(app, "/add_post").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    if (!body || !body.has("sessionID") || !body.has("content"))
        return crow::response(400, "Invalid input");

    string sid = body["sessionID"].s();
    string content = body["content"].s();

    User* user = auth.getUserBySession(sid);
    if (!user) return crow::response(401, "Unauthorized");

    user->addPost(content, auth.getAllUsers());
    return crow::response(200);
});

CROW_ROUTE(app, "/user_posts")
([&auth](const crow::request& req) {
    string sid = req.url_params.get("sid") ? req.url_params.get("sid") : "";
    User* user = auth.getUserBySession(sid);
    if (!user) return crow::response(401, "Unauthorized");

    crow::json::wvalue postList;
    const vector<Post>& posts = user->getPosts();

    for (size_t i = 0; i < posts.size(); ++i) {
        const Post& post = posts[i];

        time_t ts = post.getTimestamp();    
        tm timeinfo;
        localtime_s(&timeinfo, &ts);         

        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);

        postList[static_cast<unsigned int>(i)]["content"] = post.getContent();
        postList[static_cast<unsigned int>(i)]["timestamp"] = string(buf);
    }

    return crow::response(postList);
});

CROW_ROUTE(app, "/friends/send").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();
    string target = body["target"].s();

    User* sender = auth.getUserBySession(sid);
    User* receiver = auth.getUserByUsername(target);
    if (!sender || !receiver) return crow::response(400);

    sender->sendFriendRequest(receiver);
    return crow::response(200);
});

CROW_ROUTE(app, "/friends/search").methods("POST"_method)([&auth](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400);
    
    string sessionID = body["sessionID"].s();
    string query = body["query"].s();
    User* user = auth.getUserBySession(sessionID);
    if (!user) return crow::response(401);

    auto allUsers = auth.getAllUsers(); 
    vector<string> matches = user->searchUsersByPrefix(query);

    crow::json::wvalue res;
    std::vector<crow::json::wvalue> matchList;

    for (const string& name : matches) {
        matchList.emplace_back(name);
    }

    res["matches"] = std::move(matchList); 
    return crow::response{res};
});



CROW_ROUTE(app, "/friends/accept").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();
    string from = body["from"].s();

    User* user = auth.getUserBySession(sid);
    User* other = auth.getUserByUsername(from);
    if (!user || !other) return crow::response(400);

    user->acceptFriendRequest(other);
    return crow::response(200);
});

CROW_ROUTE(app, "/friends/reject").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();
    string from = body["from"].s();

    User* user = auth.getUserBySession(sid);
    User* other = auth.getUserByUsername(from);
    if (!user || !other) return crow::response(400);

    user->rejectFriendRequest(other);
    return crow::response(200);
});

CROW_ROUTE(app, "/friends/cancel").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();
    string to = body["to"].s();

    User* user = auth.getUserBySession(sid);
    User* other = auth.getUserByUsername(to);
    if (!user || !other) return crow::response(400);

    user->cancelFriendRequest(other);
    return crow::response(200);
});

CROW_ROUTE(app, "/friends/list").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();

    User* user = auth.getUserBySession(sid);
    if (!user) return crow::response(400);

    vector<string> list = user->getFriendList();
    crow::json::wvalue res;
    for (size_t i = 0; i < list.size(); ++i)
        res["friends"][(int)i] = list[i];

    return crow::response(res);
});

CROW_ROUTE(app, "/friends/requests").methods("POST"_method)([&auth](const crow::request& req){
    loadAllUsernamesToAVL();
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();

    User* user = auth.getUserBySession(sid);
    if (!user) return crow::response(400);

    crow::json::wvalue res;
    auto sent = user->getSentRequests();
    auto recv = user->getReceivedRequests();

    for (size_t i = 0; i < sent.size(); ++i)
        res["sent"][(int)i] = sent[i];
    for (size_t i = 0; i < recv.size(); ++i)
        res["received"][(int)i] = recv[i];

    return crow::response(res);
});

CROW_ROUTE(app, "/friends/mutual").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();
    string other = body["user"].s();

    User* user1 = auth.getUserBySession(sid);
    User* user2 = auth.getUserByUsername(other);
    if (!user1 || !user2) return crow::response(400);

    int count = user1->mutualFriendCount(user2);
    crow::json::wvalue res;
    res["count"] = count;
    return crow::response(res);
});

CROW_ROUTE(app, "/friends/suggest").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    string sid = body["sessionID"].s();

    User* user = auth.getUserBySession(sid);
    if (!user) return crow::response(400);

    auto suggestions = user->suggestFriends(auth.getAllUsers());
    crow::json::wvalue res;
    for (size_t i = 0; i < suggestions.size(); ++i)
        res["suggestions"][(int)i] = suggestions[i];

    return crow::response(res);
});

CROW_ROUTE(app, "/friends")
([]{
    ifstream file("UI/friends.html");
    stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    crow::response res;
    res.code = 200;
    res.set_header("Content-Type", "text/html");
    res.body = buffer.str();
    return res;
});

CROW_ROUTE(app, "/friends/remove").methods("POST"_method)([&auth](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400);
    string sessionID = body["sessionID"].s();
    string friendName = body["username"].s();

    User* user = auth.getUserBySession(sessionID);
    User* target = auth.getUserByUsername(friendName);
    if (!user || !target) return crow::response(404);
    
    user->removeFriend(target);
    target->removeFriend(user); 
    return crow::response(200);
});

CROW_ROUTE(app, "/timeline")
([] {
    std::ifstream file("UI/timeline.html");
if (!file.is_open()) return crow::response(404);

std::stringstream buffer;
buffer << file.rdbuf();
file.close();
return crow::response(buffer.str());

});

CROW_ROUTE(app, "/timeline").methods("POST"_method)
([&auth](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400);
    string sessionID = body["sessionID"].s();

    User* user = auth.getUserBySession(sessionID);
    if (!user) return crow::response(401);

    vector<Post> allPosts = user->getPosts();

    for (const string& friendName : user->getFriendList()) {
        User* f = auth.getUserByUsername(friendName);
        if (f) {
            const auto& theirPosts = f->getPosts();
            allPosts.insert(allPosts.end(), theirPosts.begin(), theirPosts.end());
        }
    }

    sort(allPosts.begin(), allPosts.end(), [](const Post& a, const Post& b) {
        return a.getTimestamp() > b.getTimestamp();  
    });

    crow::json::wvalue res;
    int i = 0;
    for (const auto& post : allPosts) {
    res["posts"][i]["username"] = post.getUsername();
    res["posts"][i]["content"] = post.getContent();
    res["posts"][i]["timestamp"] = static_cast<uint64_t>(post.getTimestamp());
    ++i;
    }

    return crow::response{res};
});

CROW_ROUTE(app, "/notifications").methods("POST"_method)
([&auth](const crow::request& req) {
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400);
    string sessionID = body["sessionID"].s();

    User* user = auth.getUserBySession(sessionID);
    if (!user) return crow::response(401);

    crow::json::wvalue res;
    int i = 0;
    for (const string& msg : user->getNotifications()) {
        res["notifications"][i++] = msg;
    }

    user->clearNotifications(); 
    return crow::response(res);
});

CROW_ROUTE(app, "/notifications")([] {
    std::ifstream file("UI/notifications.html");
    if (!file.is_open()) return crow::response(404);
    std::ostringstream buf;
    buf << file.rdbuf();
    return crow::response(buf.str());
});


    app.port(18080).multithreaded().run();
}