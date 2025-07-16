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

    CROW_ROUTE(app, "/login").methods("POST"_method)([&auth](const crow::request& req){
    auto body = crow::json::load(req.body);
    if (!body) return crow::response(400, "Invalid JSON");

    string uname = body["username"].s();
    string pwd = body["password"].s();

    string sid = auth.login(uname, pwd);
    if (!sid.empty()) {
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


    app.port(18080).multithreaded().run();
}