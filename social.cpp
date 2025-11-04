#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <fstream>
using namespace std;

class SocialNetwork {
private:
    unordered_map<string, vector<string>> friendsList;
    string dataFile = "network_data.txt";

    void loadData() {
        friendsList.clear();
        ifstream file(dataFile);
        if (!file) return;
        string user;
        while (file >> user) {
            int n;
            if (!(file >> n)) break;
            vector<string> f(n);
            for (int i = 0; i < n; i++) file >> f[i];
            friendsList[user] = f;
        }
        file.close();
    }

    void saveData() {
        ofstream file(dataFile);
        for (auto &p : friendsList) {
            file << p.first << " " << p.second.size();
            for (auto &f : p.second) file << " " << f;
            file << "\n";
        }
        file.close();
    }

public:
    SocialNetwork() { loadData(); }
    ~SocialNetwork() { saveData(); }

    void addUser(const string &name) {
        loadData();
        if (friendsList.find(name) == friendsList.end()) {
            friendsList[name] = {};
            cout << name << " joined the network!\n";
        } else cout << name << " already exists!\n";
        saveData();
    }

    void removeUser(const string &name) {
        loadData();
        if (!friendsList.count(name)) {
            cout << " User not found.\n";
            return;
        }
        for (auto &p : friendsList) {
            auto &flist = p.second;
            flist.erase(remove(flist.begin(), flist.end(), name), flist.end());
        }
        friendsList.erase(name);
        saveData();
        cout << " User " << name << " removed.\n";
    }

    void addFriendship(const string &user1, const string &user2) {
        loadData();
        if (friendsList.count(user1) && friendsList.count(user2)) {
            if (find(friendsList[user1].begin(), friendsList[user1].end(), user2) == friendsList[user1].end()) {
                friendsList[user1].push_back(user2);
                friendsList[user2].push_back(user1);
                cout << user1 << " and " << user2 << " are now friends!\n";
            } else cout << "They’re already friends!\n";
        } else cout << "One or both users not found.\n";
        saveData();
    }

    void removeFriendship(const string &user1, const string &user2) {
        loadData();
        if (friendsList.count(user1) && friendsList.count(user2)) {
            auto &f1 = friendsList[user1];
            auto &f2 = friendsList[user2];
            bool found = false;
            if (find(f1.begin(), f1.end(), user2) != f1.end()) {
                f1.erase(remove(f1.begin(), f1.end(), user2), f1.end());
                f2.erase(remove(f2.begin(), f2.end(), user1), f2.end());
                found = true;
            }
            if (found)
                cout << user1 << " and " << user2 << " are no longer friends.\n";
            else
                cout << "They weren’t friends.\n";
        } else cout << "One or both users not found.\n";
        saveData();
    }

    void showFriends(const string &name) {
        loadData();
        if (!friendsList.count(name)) {
            cout << " User not found!\n";
            return;
        }
        cout << " Friends of " << name << ": ";
        for (auto &f : friendsList[name]) cout << f << " ";
        cout << "\n";
    }

    void shortestPath(const string &start, const string &end) {
        loadData();
        if (!friendsList.count(start) || !friendsList.count(end)) {
            cout << " User not found!\n";
            return;
        }
        queue<vector<string>> q;
        set<string> visited;
        q.push({start});
        visited.insert(start);
        while (!q.empty()) {
            auto path = q.front(); q.pop();
            string last = path.back();
            if (last == end) {
                cout << "🔗 Shortest path: ";
                for (auto &p : path) cout << p << " ";
                cout << "\n";
                return;
            }
            for (auto &f : friendsList[last]) {
                if (!visited.count(f)) {
                    visited.insert(f);
                    auto newPath = path;
                    newPath.push_back(f);
                    q.push(newPath);
                }
            }
        }
        cout << "No connection found.\n";
    }

    void recommendFriends(const string &name) {
        loadData();
        if (!friendsList.count(name)) {
            cout << " User not found!\n";
            return;
        }

        unordered_map<string, int> suggestionCount;
        auto &myFriends = friendsList[name];
        unordered_set<string> mySet(myFriends.begin(), myFriends.end());

        for (auto &friendName : myFriends) {
            for (auto &mutual : friendsList[friendName]) {
                if (mutual != name && !mySet.count(mutual)) {
                    suggestionCount[mutual]++;
                }
            }
        }

        if (suggestionCount.empty()) {
            cout << " No friend suggestions for " << name << ".\n";
            return;
        }

        cout << " Suggestions for " << name << ":\n";
        for (auto &pair : suggestionCount)
            cout << " - " << pair.first << " (" << pair.second << " mutual friends)\n";
    }
};

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    static SocialNetwork sn;

    if (argc < 2) {
        cout << " No command provided.\n";
        cout.flush();
        return 0;
    }

    string cmd = argv[1];

    if (cmd == "add_user" && argc == 3) sn.addUser(argv[2]);
    else if (cmd == "remove_user" && argc == 3) sn.removeUser(argv[2]);
    else if (cmd == "add_friend" && argc == 4) sn.addFriendship(argv[2], argv[3]);
    else if (cmd == "remove_friend" && argc == 4) sn.removeFriendship(argv[2], argv[3]);
    else if (cmd == "show_friends" && argc == 3) sn.showFriends(argv[2]);
    else if (cmd == "shortest_path" && argc == 4) sn.shortestPath(argv[2], argv[3]);
    else if (cmd == "recommend" && argc == 3) sn.recommendFriends(argv[2]);
    else cout << " Invalid command or arguments.\n";

    cout.flush(); // ✅ ensures Flask always receives the output
    return 0;
}
