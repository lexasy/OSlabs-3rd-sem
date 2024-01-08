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
#define GAME_SEMAPHORE "semaphore"
#define SERVER_PID_IN_MEMORY "pid"
#define SIZE 4096

struct Player {
    std::string name;
    int bulls = 0;
    int cows = 0;
    int supposition = 0;
    bool win = false;
    pid_t pid;
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
    std::vector<Player> players_list;
    std::string answer;
};

std::ostream& operator<<(std::ostream& os, const Session& session) {
    os << "Session name: " << session.session_name << "\n";
    os << "Quantity of players: " << session.players_list.size() << "\n";
    os << "Players:\n";
    for (auto i : session.players_list) {
        os << i << "\n";
    }
    os << "Answer: " << session.answer << "\n";
    return os;
}

int random_number() {
    srand(time(NULL));
    return rand() % 900 + 100;
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