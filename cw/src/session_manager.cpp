#include "game_configuration.hpp"

static int flag = 1;
static int exit_flag = 1;
static int winner_flag = 1;

std::map<std::string, pid_t> players_map;
std::map<std::string, Session> sessions;

void signal_handler(int signum) {
    flag = 0;
}

void exit_handler(int signum) {
    exit_flag = 0;
    for (auto i : players_map) {
        kill(i.second, SIGINT);
    }
    std::cout << "\n";
    exit(0);
}

void winner_handler(int signum) {
    winner_flag = 0;
}

pid_t create_process() {
    pid_t pid = fork();
    if (-1 == pid) {
        perror("fork");
        exit(-1);
    }
    return pid;
}

bool member(const Session& session, std::vector<Session> arr) {
    for (int i = 0; i < arr.size(); i++) {
        if (session == arr[i]) {
            return true;
        }
    }
    return false;
}

bool not_full_sess(std::map<std::string, Session> sessions) {

    for (auto i : sessions) {
        if (i.second.players_list.size() < i.second.max_players_quantity) {
            return false;
        } 
    }
    return true;
}

void *winner(void *arg) {
    int manth_fd = shm_open(MANAGER_WINNER_THREAD_PID_IN_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(manth_fd, SIZE);
    void *manth_pid_mmap = mmap(NULL, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, manth_fd, 0);
    strcpy((char *)manth_pid_mmap, std::to_string(getpid()).c_str());
    munmap(manth_pid_mmap, SIZE);
    close(manth_fd);
    int man_fd = shm_open(MANAGER_WINNER_THREAD_CLIENT_COMMUNICATION, O_CREAT | O_RDWR, 0666);
    ftruncate(man_fd, SIZE);
    char *session_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, man_fd, 0));
    while (1) {
        signal(SIGUSR2, winner_handler);
        while (winner_flag) {sleep(1);}
        winner_flag = 1;
        auto sess = sessions.find(std::string(session_mmap));
        for (int i = 0; i < sess->second.players_list.size(); i++) {
            players_map.erase(sess->second.players_list[i]);
        }
        sessions.erase(std::string(session_mmap));
        std::cout << "[" << toup(std::string(session_mmap)) << "] Session " << std::string(session_mmap) << " was closed!\n\n";
    }
}

void *manager(void *arg) {
    int pid_fd = shm_open(MANAGER_PID_IN_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(pid_fd, SIZE);
    void *pid_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, pid_fd, 0);
    strcpy((char *)pid_mmap, std::to_string(getpid()).c_str());
    munmap(pid_mmap, SIZE);
    close(pid_fd);
    int fd = shm_open(MANAGER_CLIENT_COMMUNICATION, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    char *session_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0));
    signal(SIGINT, exit_handler);
    while (exit_flag) {
        signal(SIGUSR1, signal_handler); 
        while (flag) {sleep(1);}
        nlohmann::json session_action = nlohmann::json::parse(std::string(session_mmap));
        if (players_map.find(session_action["player"]) == players_map.end()) {
            players_map[session_action["player"]] = session_action["pid"].get<pid_t>();
        }
        if (session_action["action"] == "create") {
            nlohmann::json reply {};
            if (sessions.find(session_action["name"].get<std::string>()) != sessions.end()) {
                reply["ok"] = false;
                reply["desc"] = "Session with the same name already exists!";
            } else {
                Session sess;
                sess.session_name = session_action["name"];
                sess.max_players_quantity = session_action["max"];
                sess.players_list.push_back(session_action["player"].get<std::string>());
                sessions[sess.session_name] = sess;
                pid_t pid = create_process();
                reply["ok"] = true;
                reply["desc"] = "Session " + session_action["name"].get<std::string>() + " was created successfully!";
                if (pid == 0) {
                    char *argv[] = {const_cast<char*>(sess.session_name.c_str()), (char *) NULL};
                    execv("server", argv);
                }
            }
            strcpy(session_mmap, reply.dump().c_str());
            kill(session_action["pid"], SIGUSR1);
            flag = 1;
        } else if (session_action["action"] == "join") {
            std::string session_name = session_action["name"];
            nlohmann::json reply {};
            if (sessions.find(session_name) == sessions.end()) {
                reply["ok"] = false;
                reply["desc"] = "Session not found!";
            } else {
                Session dest = sessions.find(session_name)->second;
                if (dest.players_list.size() == dest.max_players_quantity) {
                    reply["ok"] = false;
                    reply["desc"] = "Not more free places in session!";
                } else {
                    dest.players_list.push_back(session_action["player"]);
                    sessions.find(session_name)->second.players_list = dest.players_list;
                    reply["ok"] = true;
                    reply["desc"] = "You successfully joined to session " + session_action["name"].get<std::string>() + "!";
                }
            }
            strcpy(session_mmap, reply.dump().c_str());
            kill(session_action["pid"], SIGUSR1);
            flag = 1;
        } else if (session_action["action"] == "find") {
            nlohmann::json reply {};
            if (sessions.size() == 0 || not_full_sess(sessions)) {
                reply["ok"] = false;
                reply["desc"] = "Free sessions not found!";
            } else {
                for (auto& i : sessions) {
                    if (i.second.players_list.size() != i.second.max_players_quantity) {
                        i.second.players_list.push_back(session_action["player"]);
                        reply["ok"] = true;
                        reply["desc"] = "You successfully joined to session " + i.first + "!";
                        reply["session"] = i.first;
                        break;
                    }
                }
            }
            strcpy(session_mmap, reply.dump().c_str());
            kill(session_action["pid"], SIGUSR1);
            flag = 1;
        }      
    }
    munmap(session_mmap, SIZE);
    close(fd);
}

int main() {
    pthread_t manager_thread, win_thread;
    pthread_create(&manager_thread, NULL, manager, NULL);
    pthread_create(&win_thread, NULL, winner, NULL);
    pthread_join(manager_thread, NULL);
    pthread_join(win_thread, NULL);
}