#include "../include/socket.hpp"

int main(int argc, char *argv[]) {
    if (argc != 2 && argc != 3) {
        throw std::runtime_error("Wrong quantity of arguments for calculator node");
    }
    int curr_id = atoi(argv[1]);
    int child_id = (argc == 3 ? atoi(argv[2]) : -1);
    std::string address;
    std::unordered_map<std::string, int> dictionary;
    zmq::context_t context;
    zmq::socket_t parent_socket(context, ZMQ_REP);
    connect(parent_socket, curr_id);
    zmq::socket_t child_socket(context, ZMQ_REQ);
    if (child_id == -1) {
        bind(child_socket, child_id);
    }
    child_socket.set(zmq::sockopt::sndtimeo, 5000);
    std::string message;
    while (true) {
        message = receive_message(parent_socket);
        std::istringstream request(message);
        int destination_id; std::string command;
        request >> destination_id;
        request >> command;
        if (destination_id == curr_id) {
            if (command == "pid") {
                send_message(parent_socket, "OK: " + std::to_string(getpid()));
            } else if (command == "create") {
                int newchild_id;
                request >> newchild_id;
                if (child_id != -1) {
                    unbind(child_socket, child_id);
                }
                bind(child_socket, newchild_id);
                pid_t pid = fork();
                if (pid < 0) {
                    std::cout << "Can't create a new process\n";
                    return -1;
                }
                if (pid == 0) {
                    execl("../build/calculator", "../build/calculator", std::to_string(newchild_id).c_str(), std::to_string(child_id).c_str(), NULL);
                    std::cout << "Can't execute a new process\n";
                    return -1;
                }
                send_message(child_socket, std::to_string(newchild_id) + " pid");
                child_id = newchild_id;
                send_message(parent_socket, receive_message(child_socket));
            } else if (command == "check") {
                std::string key;
                request >> key;
                if (dictionary.find(key) != dictionary.end()) {
                    send_message(parent_socket, "OK: " + std::to_string(curr_id) + ": " + std::to_string(dictionary[key]));
                } else {
                    send_message(parent_socket, "OK: " + std::to_string(curr_id) + ": '" + key + "' not found");
                }
            } else if (command == "ping") {
                std::string reply;
                if (child_id != -1) {
                    send_message(child_socket, std::to_string(child_id) + " ping");
                    std::string msg = receive_message(child_socket);
                    reply += " " + msg;
                }
                send_message(parent_socket, std::to_string(curr_id) + reply);
            } else if (command == "add") {
                std::string key;
                int value;
                request >> key >> value;
                dictionary[key] = value;
                send_message(parent_socket, "OK: " + std::to_string(curr_id));
            } else if (command == "kill") {
                if (child_id != -1) {
                    send_message(child_socket, std::to_string(child_id) + " kill");
                    std::string msg = receive_message(child_socket);
                    if (msg == "OK") {
                        send_message(parent_socket, "OK");
                    }
                    unbind(child_socket, child_id);
                    disconnect(parent_socket, curr_id);
                    break;
                }
                send_message(parent_socket, "OK");
                disconnect(parent_socket, curr_id);
                break;
            }
        } else if (child_id != -1) {
            send_message(child_socket, message);
            send_message(parent_socket, receive_message(child_socket));
            if (child_id == destination_id && command == "kill") {
                child_id = -1;
            }
        } else {
            send_message(parent_socket, "Error: Node is unavailable");
        }
    }
}