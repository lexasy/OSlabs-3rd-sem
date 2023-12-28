#include "../include/socket.hpp"

void sendMessage(zmq::socket_t& socket, const std::string& msg) {
    zmq::message_t message(msg.size());
    memcpy(message.data(), msg.c_str(), msg.size());
    socket.send(message, zmq::send_flags::none);
}


std::string receiveMessage(zmq::socket_t& socket) {
    zmq::message_t msg;
    int msgReceiv;
    try {
        std::optional<size_t> result = socket.recv(msg);
        if (result) {
            msgReceiv = static_cast<int>(*result);
        }
    }
    catch (...) {
        msgReceiv = 0;
    }
    if (msgReceiv == 0) {
        return "Error: Node is unavailable";
    }
    std::string receivedMsg(static_cast<char*>(msg.data()), msg.size());
    return receivedMsg;

}

void connect(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.connect(address);
}

void disconnect(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.disconnect(address);
}

void bind(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.bind(address);
}

void unbind(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(MAIN_PORT + id);
    socket.unbind(address);
}