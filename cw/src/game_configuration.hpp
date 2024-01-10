#pragma once

#include <bits/stdc++.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstring>

#define GAME_FILENAME "game"
#define SERVER_PID_IN_MEMORY "server_pid"
#define MANAGER_PID_IN_MEMORY "manger_pid"
#define MANAGER_CLIENT_COMMUNICATION "manager"
#define MANAGER_WINNER_THREAD_PID_IN_MEMORY "manager_winner_pid"
#define MANAGER_WINNER_THREAD_CLIENT_COMMUNICATION "manager_winner"
#define SIZE 4096

struct Player {
    std::string name;
    int bulls = 0;
    int cows = 0;
    int supposition = 0;
    bool win = false;
    pid_t pid;
    std::string sess_name;
    Player(std::string _name) {
        name = _name;
        pid = getpid();
    }     
};

std::ostream& operator<<(std::ostream& os, const Player& player) {
    os << "bulls: " << player.bulls << "\n";
    os << "cows: " << player.cows << "\n";
    return os;
}

struct Session {
    std::string session_name;
    std::vector<std::string> players_list;
    int max_players_quantity;
    // std::string answer;
};

// std::ostream& operator<<(std::ostream& os, const Session& session) {
//     os << "Session name: " << session.session_name << "\n";
//     os << "Quantity of players: " << session.players_list.size() << "\n";
//     os << "Players:\n";
//     for (auto i : session.players_list) {
//         os << i << "\n";
//     }
//     // os << "Answer: " << session.answer << "\n";
//     return os;
// }

bool operator==(const Session& session1, const Session& session2) {
    return session1.session_name == session2.session_name;
}

int random_number() {
    srand(time(NULL));
    return rand() % 900 + 100;
}

std::string toup(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return str;
}

void game(nlohmann::json& player_stats, std::string answer) {
    std::string supposition = std::to_string(player_stats["supposition"].get<int>());
    int cows = 0; int bulls = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (answer[i] == supposition[j]) {
                if (i == j) {
                    bulls++;
                } else {
                    cows++;
                }
            }
        }
    }
    if (bulls == 3) {
        player_stats["win"] = true;
    }
    player_stats["bulls"] = bulls;
    player_stats["cows"] = cows;
}
