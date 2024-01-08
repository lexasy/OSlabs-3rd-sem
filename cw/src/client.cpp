#include "game_configuration.hpp"

static int flag = 1;
static int winner_flag = 1;

void signal_handler(int sugnum) {
    flag = 0;
}

void winner_signal_handler(int signum) {
    winner_flag = 0;
}

void *winner_handler(void *arg) {
    signal(SIGUSR2, winner_signal_handler);
    while (winner_flag) {sleep(1);}
    int game_fd = shm_open(GAME_FILENAME, O_CREAT | O_RDWR, 0666);
    ftruncate(game_fd, SIZE);
    char *game_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, game_fd, 0));
    std::cout << "\n" << std::string(game_mmap) << "\n";
    sleep(1);
    munmap(game_mmap, SIZE);
    close(game_fd);
    shm_unlink(GAME_FILENAME);
    exit(0);
}

void *client(void *arg) {
    int pid_fd = shm_open(SERVER_PID_IN_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(pid_fd, SIZE);
    void *pid_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, pid_fd, 0);
    pid_t server_pid = std::atoi((char *) pid_mmap);
    int game_fd = shm_open(GAME_FILENAME, O_CREAT | O_RDWR, 0666);
    ftruncate(game_fd, SIZE);
    char *game_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, game_fd, 0));
    std::string name;
    std::cout << "Input your name: ";
    std::cin >> name;
    Player player(name);
    nlohmann::json stats {};
    stats["name"] = player.name;
    while (1) {
        int supposition;
        std::cout << "Input your supposition: ";
        std::cin >> supposition;
        player.supposition = supposition;
        stats["bulls"] = player.bulls;
        stats["cows"] = player.cows;
        stats["supposition"] = player.supposition;
        stats["win"] = player.win;
        stats["pid"] = player.pid;
        strcpy(game_mmap, stats.dump().c_str());
        kill(server_pid, SIGUSR1);
        signal(SIGUSR1, signal_handler);
        while (flag) {sleep(1);}
        flag = 1;
        std::string message = std::string(game_mmap);
        stats = nlohmann::json::parse(message);
        player.bulls = stats["bulls"];
        player.cows = stats["cows"];
        player.win = stats["win"];
        std::cout << "\n" << player << "\n";
    }
    munmap(game_mmap, SIZE);
    close(game_fd);
    shm_unlink(GAME_FILENAME);
    return nullptr;
}

int main() {
    pthread_t client_thread, signal_thread;
    pthread_create(&client_thread, NULL, client, NULL);
    pthread_create(&signal_thread, NULL, winner_handler, NULL);
    pthread_join(client_thread, NULL); pthread_join(signal_thread, NULL);
}

