#include "game_configuration.hpp"

static int flag = 1;
static int winner_flag = 1;
static int exit_flag = 1;

void signal_handler(int sugnum) {
    flag = 0;
}

void winner_signal_handler(int signum) {
    winner_flag = 0;
}

void exit_signal_handler(int signum) {
    exit_flag = 0;
}

void *exit_handler(void *arg) {
    signal(SIGINT, exit_signal_handler);
    while (exit_flag) {sleep(1);}
    std::cout << "\n";
    exit(0);
}

void *winner_handler(void *arg) {
    std::string *session = (std::string *) arg;
    signal(SIGUSR2, winner_signal_handler);
    while (winner_flag) {sleep(1);}
    int game_fd = shm_open((*session).c_str(), O_CREAT | O_RDWR, 0666);
    ftruncate(game_fd, SIZE);
    char *game_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, game_fd, 0));
    std::cout << "\n" << "[INFO] " << std::string(game_mmap) << "\n";
    sleep(1);
    munmap(game_mmap, SIZE);
    close(game_fd);
    shm_unlink((*session).c_str());
    int manth_fd = shm_open(MANAGER_WINNER_THREAD_PID_IN_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(manth_fd, SIZE);
    void *manth_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, manth_fd, 0);
    pid_t manth_pid = std::atoi((char *) manth_mmap);
    int man_fd = shm_open(MANAGER_WINNER_THREAD_CLIENT_COMMUNICATION, O_CREAT | O_RDWR, 0666);
    ftruncate(man_fd, SIZE);
    void *man_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, man_fd, 0);
    strcpy((char *) man_mmap, (const_cast<char *>((*session).c_str())));
    kill(manth_pid, SIGUSR2);
    exit(0);
}

void *client(void *arg) {
    sleep(1);
    Player player = *((Player *) arg);
    std::string pid_sess = std::string(SERVER_PID_IN_MEMORY) + player.sess_name;
    int pid_fd = shm_open(pid_sess.c_str(), O_CREAT | O_RDWR, 0666);
    ftruncate(pid_fd, SIZE);
    void *pid_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, pid_fd, 0);
    pid_t server_pid = std::atoi((char *) pid_mmap);
    int game_fd = shm_open(player.sess_name.c_str(), O_CREAT | O_RDWR, 0666);
    ftruncate(game_fd, SIZE);
    char *game_mmap = static_cast<char *>(mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, game_fd, 0));
    nlohmann::json stats {};
    stats["name"] = player.name;
    stats["session"] = player.sess_name;
    while (1) {
        int supposition;
        std::cout << "[CLIENT] Input your supposition: ";
        std::cin >> supposition;
        player.supposition = supposition;
        stats["bulls"] = player.bulls;
        stats["cows"] = player.cows;
        stats["supposition"] = player.supposition;
        stats["win"] = player.win;
        stats["pid"] = player.pid;
        std::cout << stats.dump() << "\n";
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
        std::cout << "\n[INFO] bulls: " << player.bulls << "\n";
        std::cout << "[INFO] cows: " << player.cows << "\n\n";
    }
    munmap(game_mmap, SIZE);
    close(game_fd);
    shm_unlink(player.sess_name.c_str());
}

int main() {
    int man_pid_fd = shm_open(MANAGER_PID_IN_MEMORY, O_CREAT | O_RDWR, 0666);
    ftruncate(man_pid_fd, SIZE);
    void *man_pid_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, man_pid_fd, 0);
    pid_t man_pid = std::atoi((char *) man_pid_mmap);

    int man_fd = shm_open(MANAGER_CLIENT_COMMUNICATION, O_CREAT | O_RDWR, 0666);
    ftruncate(man_fd, SIZE);
    void *man_mmap = mmap(NULL, SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, man_fd, 0);

    std::string name;
    std::cout << "[CLIENT] Input your name: ";
    std::cin >> name;
    Player player(name);
    std::cout << "\n[INFO] create [session name] [max players quantity]\n";
    std::cout << "[INFO] join [session name]\n";
    std::cout << "[INFO] find\n\n";
    std::string sess_name;
    while (true) {
        std::cout << "[COMMAND] ";
        std::string command;
        std::cin >> command;
        if (command == "create") {
            nlohmann::json request {};
            request["action"] = "create";
            request["player"] = name;
            request["pid"] = player.pid;
            std::string session_name;
            std::cin >> session_name;
            sess_name = session_name;
            request["name"] = session_name;
            int maxc; std::cin >> maxc;
            request["max"] = maxc;
            strcpy((char *) man_mmap, request.dump().c_str());
            kill(man_pid, SIGUSR1);
            signal(SIGUSR1, signal_handler);
            while (flag) {sleep(1);}
            flag = 1;
            nlohmann::json reply = nlohmann::json::parse(std::string((char *)man_mmap));
            std::cout << "[INFO] " << reply["desc"].get<std::string>() << "\n\n";
            if (!reply["ok"]) {
                continue;
            } else {
                break;
            }
        } else if (command == "join") {
            nlohmann::json request {};
            request["action"] = "join";
            request["player"] = name;
            request["pid"] = player.pid;
            std::string session_name;
            std::cin >> session_name;
            sess_name = session_name;
            request["name"] = session_name;
            strcpy((char *) man_mmap, request.dump().c_str());
            kill(man_pid, SIGUSR1);
            signal(SIGUSR1, signal_handler);
            while (flag) {sleep(1);}
            flag = 1;
            nlohmann::json reply = nlohmann::json::parse(std::string((char *)man_mmap));
            std::cout << "[INFO] " << reply["desc"].get<std::string>() << "\n\n";
            if (!reply["ok"]) {
                continue;
            } else {
                break;
            }
        } else if (command == "find") {
            nlohmann::json request {};
            request["action"] = "find";
            request["player"] = name;
            request["pid"] = player.pid;
            strcpy((char *) man_mmap, request.dump().c_str());
            kill(man_pid, SIGUSR1);
            signal(SIGUSR1, signal_handler);
            while (flag) {sleep(1);}
            flag = 1;
            nlohmann::json reply = nlohmann::json::parse(std::string((char *)man_mmap));
            std::cout << "[INFO] " << reply["desc"].get<std::string>() << "\n\n";
            if (!reply["ok"]) {
                continue;
            } else {
                sess_name = reply["session"];
                break;
            }
        } else {
            std::cout << "[INFO] Incorrect command!\n";
        }
    }
    player.sess_name = sess_name;

    pthread_t client_thread, signal_thread, exit_thread;
    pthread_create(&client_thread, NULL, client, &player);
    pthread_create(&signal_thread, NULL, winner_handler, &sess_name);
    pthread_create(&exit_thread, NULL, exit_handler, NULL);
    pthread_join(client_thread, NULL);
    pthread_join(signal_thread, NULL);
    pthread_join(exit_thread, NULL);
}

