#include "game_configuration.hpp"

static int flag = 1;

void signal_handler(int signum) {
    flag = 0;
}

bool member(std::vector<pid_t> arr, pid_t pid) {
    for (int i = 0; i < arr.size(); i++) {
        if (pid == arr[i]) {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    int answer = random_number();
    std::vector<pid_t> clients_pid;
    std::cout << "[" << toup(std::string(argv[0])) << "] Answer in session " << std::string(argv[0]) << " is " << answer << "\n\n";
    std::string pid_sess = std::string(SERVER_PID_IN_MEMORY) + std::string(argv[0]);
    int pid_fd = shm_open(pid_sess.c_str(), O_CREAT | O_RDWR, 0666);
    ftruncate(pid_fd, SIZE);
    void *pid_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, pid_fd, 0);
    strcpy((char *)pid_mmap, std::to_string(getpid()).c_str());
    munmap(pid_mmap, SIZE);
    close(pid_fd);
    int game_fd = shm_open(argv[0], O_CREAT | O_RDWR, 0666);
    ftruncate(game_fd, SIZE);
    char *game_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, game_fd, 0));
    while (1) {
        signal(SIGUSR1, signal_handler);
        while (flag) {sleep(1);}
        std::string message = std::string(game_mmap);
        std::cout << "[RECEIVED] " << message << "\n";
        nlohmann::json stats = nlohmann::json::parse(message);
        if (!member(clients_pid, stats["pid"])) {
            clients_pid.push_back(stats["pid"]);
        }
        strcpy(game_mmap, "");
        game(stats, std::to_string(answer));
        std::cout << "[SENT] " << stats.dump() << "\n\n";
        if (stats["win"] == true) {
            std::string reply = "Game over! Player " + stats["name"].get<std::string>() + " won!";
            strcpy(game_mmap, reply.c_str());
            for (auto pid : clients_pid) {
                kill(pid, SIGUSR2);
                flag = 1;
            }
        } else {
            strcpy(game_mmap, stats.dump().c_str());
            kill(stats["pid"], SIGUSR1);
            flag = 1;
        }       
    }
    munmap(game_mmap, SIZE);
    close(game_fd);
    shm_unlink(argv[0]);
}