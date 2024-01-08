#include "game_configuration.hpp"

static int flag = 1;

void signal_handler(int signum) {
    flag = 0;
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
        if (i.second.players_list.size() != i.second.max_players_quantity) {
            return false;
        } 
    }
    return true;
}

int main() {
    std::map<std::string, Session> sessions;
    int pid_fd = shm_open(MANAGER_PID_IN_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(pid_fd, SIZE);
    void *pid_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, pid_fd, 0);
    strcpy((char *)pid_mmap, std::to_string(getpid()).c_str());
    munmap(pid_mmap, SIZE);
    close(pid_fd);
    int fd = shm_open(MANAGER_CLIENT_COMMUNICATION, O_CREAT | O_RDWR, 0666);
    ftruncate(fd, SIZE);
    char *session_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0));
    while (1) {
        signal(SIGUSR1, signal_handler); 
        while (flag) {sleep(1);}
        nlohmann::json session_action = nlohmann::json::parse(std::string(session_mmap));
        std::cout << std::string(session_mmap) << "\n";
        if (session_action["action"] == "create") {
            Session sess;
            sess.session_name = session_action["name"];
            sess.max_players_quantity = session_action["max"];
            sess.players_list.push_back(session_action["player"].get<std::string>());
            sessions[sess.session_name] = sess;
            pid_t pid = create_process();
            if (pid == 0) {
                char *argv[] = {const_cast<char*>(sess.session_name.c_str()), (char *) NULL};
                execv("server", argv);
            }
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
                    reply["ok"] == false;
                    reply["desc"] = "Not more free places in session!";
                } else {
                    dest.players_list.push_back(session_action["player"]);
                    reply["ok"] = true;
                }
            }
            strcpy(session_mmap, reply.dump().c_str());
            kill(session_action["pid"], SIGUSR1);
            flag = 1;
        } else if (session_action["action"] == "find") {
            nlohmann::json reply {};
            if (sessions.size() == 0 && not_full_sess(sessions)) {
                reply["ok"] = false;
                reply["desc"] = "Free sessions not found";
            } else {
                for (auto i : sessions) {
                    if (i.second.players_list.size() != i.second.max_players_quantity) {
                        i.second.players_list.push_back(session_action["player"]);
                        reply["ok"] = true;
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