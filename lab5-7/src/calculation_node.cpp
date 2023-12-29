#include "../include/socket.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2 && argc != 3) {
        throw std::runtime_error("Wrong args for counting node");
    }

    int curId = atoi(argv[1]);
    int childId = -1;
    if (argc == 3) {
        childId = atoi(argv[2]);
    }
    std::string adr;
    std::unordered_map<std::string, int> dictionary;

    zmq::context_t context;
    zmq::socket_t parentSocket(context, ZMQ_REP);
    
    connect(parentSocket, curId);

    zmq::socket_t childSocket(context, ZMQ_REQ);
    if (childId != -1) {
        bind(childSocket, childId);
    }
    childSocket.set(zmq::sockopt::sndtimeo, 5000);
 
    std::string message;
    while (true) {
        message = receiveMessage(parentSocket);
        std::istringstream request(message);
        int destId;
        request >> destId;

        std::string command;
        request >> command;

        if (destId == curId) {
    
            if (command == "pid") {
                sendMessage(parentSocket, "OK: " + std::to_string(getpid()));
            } else if (command == "create") {

                int new_childId;
                request >> new_childId;
                if (childId != -1) {
                    unbind(childSocket, childId);
                }
                bind(childSocket, new_childId);
                pid_t pid = fork();
                if (pid < 0) {
                    std::cout << "Can't create new process" << std::endl;
                    return -1;
                }
                if (pid == 0) {
                    execl("../build/server", "../build/server", std::to_string(new_childId).c_str(), std::to_string(childId).c_str(), NULL);
                    std::cout << "Can't execute new process" << std::endl;
                    return -2;
                }
                sendMessage(childSocket, std::to_string(new_childId) + " pid");
                childId = new_childId;
                sendMessage(parentSocket, receiveMessage(childSocket));

            } else if (command == "check") {
                std::string key;
                request >> key;
                if (dictionary.find(key) != dictionary.end()) {
                    sendMessage(parentSocket, "OK: " + std::to_string(curId) + ": " + std::to_string(dictionary[key]));
                } else {
                    sendMessage(parentSocket, "OK: " + std::to_string(curId) + ": '" + key + "' not found");
                }
            } else if (command == "add") {
                std::string key;
                int value;
                request >> key >> value;
                dictionary[key] = value;
                sendMessage(parentSocket, "OK: " + std::to_string(curId));
            }  else if (command == "ping") {
                std::string reply;
                if (childId != -1) {
                    sendMessage(childSocket, std::to_string(childId) + " ping");
                    std::string msg = receiveMessage(childSocket);
                    reply += " " + msg;
                }
                sendMessage(parentSocket, std::to_string(curId) + reply);
            } else if (command == "kill") {
                if (childId != -1) {
                    sendMessage(childSocket, std::to_string(childId) + " kill");
                    std::string msg = receiveMessage(childSocket);
                    if (msg == "OK") {
                        sendMessage(parentSocket, "OK");
                    }
                    unbind(childSocket, childId);
                    disconnect(parentSocket, curId);
                    break;
                }
                sendMessage(parentSocket, "OK");
                disconnect(parentSocket, curId);
                break;
            }
        }      
         else if (childId != -1) {
            sendMessage(childSocket, message);
            sendMessage(parentSocket, receiveMessage(childSocket));
            if (childId == destId && command == "kill") {
                childId = -1;
            }
        } else {
            sendMessage(parentSocket, "Error: Node is unavailable");
        }
    }
}