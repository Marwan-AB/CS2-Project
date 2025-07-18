#include "global.h"
#include <fstream>
#include <sstream>
using namespace std;
AVLTree<string> allUsernames;




void loadAllUsernamesToAVL() {
    allUsernames.clear();  
    ifstream file("users.txt");
    string line;

    while (getline(file, line)) {
        stringstream ss(line);
        string username;
        if (std::getline(ss, username, ',')) {
            allUsernames.insert(username);
        }
    }
}