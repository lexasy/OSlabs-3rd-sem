#include "../include/topology.hpp"
#include "../include/socket.hpp"

int main() {
    Topology list;
    std::vector<zmq::socket_t> branches;
    zmq::context_t context;
    std::string command;
    while (true) {
        std::cin >> command;
        if (command == "create") {
            int node_id, parent_id;
            std::cin >> node_id >> parent_id;
            if (list.find(node_id) != -1) {
                std::cout << "Error: Already exists\n";
            } else if (parent_id == -1) {
                pid_t pid = fork();
                if (pid < 0) {
                    std::cout << "Can't create a new process\n";
                    return -1;
                } else if (pid == 0) {
                    execl("../build/calculator", "../build/calculator", std::to_string(node_id).c_str(), NULL);
                    std::cout << "Can't execute a new process\n";
                    return -1;
                }
                branches.emplace_back(context, ZMQ_REQ);
                branches[branches.size() - 1].set(zmq::sockopt::sndtimeo, 5000);
                bind(branches[branches.size() - 1], node_id);
                send_message(branches[branches.size() - 1], std::to_string(node_id) + " pid");
                std::string reply = receive_message(branches[branches.size() - 1]);
                std::cout << reply << "\n";
                list.insert(node_id, parent_id);
            } else if (list.find(parent_id) == -1) {
                std::cout << "Error: parent not found\n"; 
            } else {
                int branch = list.find(parent_id);
                send_message(branches[branch], std::to_string(parent_id) + "create " + std::to_string(node_id));
                std::string reply = receive_message(branches[branch]);
                std::cout << reply << "\n";
                list.insert(node_id, parent_id);
            }
        } else if (command == "exec") {
            std::string str; getline(std::cin, str);
            std::string exec_command;
            std::vector<std::string> tmp;
            std::string tmp1 = " ";
            for (int i = 1; i < str.size(); i++) {
                tmp1 += str[i];
                if (str[i] == ' ' || i == str.size() - 1) {
                    tmp.push_back(tmp1);
                    tmp1 = " ";
                }
            }
            if (tmp.size() == 2) {
                exec_command = "check";
            } else {
                exec_command = "add";
            }
            int destination_id = stoi(tmp[0]);
            int branch = list.find(destination_id);
            if (branch == -1) {
                std::cout << "There is no such node id\n";
            } else {
                if (exec_command == "check") {
                    send_message(branches[branch], tmp[0] + "check" + tmp[1]);
                } else if (exec_command == "add") {
                    send_message(branches[branch], tmp[0] + "add" + tmp[1] + " " + tmp[2]);
                }
                std::string reply = receive_message(branches[branch]);
                std::cout << reply << "\n";
            }
        } else if (command == "kill") {
            int id;
            std::cin >> id;
            int branch = list.find(id);
            if (branch == -1) {
                std::cout << "Error: incorrect node id\n";
            } else {
                bool is_first = (list.get_first_id(branch) == id);
                send_message(branches[branch], std::to_string(id) + "kill");
                std::string reply = receive_message(branches[branch]);
                std::cout << reply << "\n";
                list.erase(id);
                if (is_first) {
                    unbind(branches[branch], id);
                    branches.erase(branches.begin() + branch);
                }
            }
        } else if (command == "ping") {
            int node_id;
            std::cin >> node_id;
            std::set<int> available_nodes;
            if (list.find(node_id) == -1) {
                std::cout << "Error: node not found\n"; 
            } else {
                for (int i = 0; i < branches.size(); i++) {
                    int first_node_id = list.get_first_id(i);
                    send_message(branches[i], std::to_string(first_node_id) + " ping");
                    std::string received_message = receive_message(branches[i]);
                    std::istringstream reply(received_message);
                    int node;
                    while(reply >> node) {
                        available_nodes.insert(node);
                    }
                }
                if (available_nodes.empty()) {
                    std::cout << "OK: 0\n";
                } else {
                    if (available_nodes.find(node_id) != available_nodes.end()) {
                        std::cout << "OK: 1\n";
                    }
                }
            }
        } else if (command == "exit") {
            for (int i = 0; i < branches.size(); ++i) {
                int first_node_id = list.get_first_id(i);
                send_message(branches[i], std::to_string(first_node_id) + " kill");
                std::string reply = receive_message(branches[i]);
                if (reply != "OK") {
                    std::cout << reply << "\n";
                } else {
                    unbind(branches[i], first_node_id);
                }
            }
            exit(0);
        } else {
            std::cout << "Not correct command\n";
        }
    }
}