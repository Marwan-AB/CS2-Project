#include <crow.h>
#include "Authentication.h"
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

    user->addPost(content);
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

    app.port(18080).multithreaded().run();
}