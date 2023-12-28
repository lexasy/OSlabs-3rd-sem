#include "../include/socket.hpp"

void send_message(zmq::socket_t& socket, const std::string& message) {
    zmq::message_t msg(message.size());
    memcpy(msg.data(), message.c_str(), message.size());
    socket.send(msg, zmq::send_flags::none);
}

std::string receive_message(zmq::socket_t& socket) {
    zmq::message_t message;
    int receiving;
    try {
        std::optional<size_t> res = socket.recv(message);
        if (res) {
            receiving = static_cast<int>(*res);
        }
    }
    catch (...) {
        receiving = 0;
    }
    if (receiving == 0) {
        return "Error: Node is unavailable lee";
    }
    std::string received_message(static_cast<char *>(message.data()), message.size());
    return received_message;
}

void connect(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(PORT + id);
    socket.connect(address);
}

void disconnect(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(PORT + id);
    socket.disconnect(address);
}

void bind(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(PORT + id);
    socket.bind(address);
}

void unbind(zmq::socket_t& socket, int id) {
    std::string address = "tcp://127.0.0.1:" + std::to_string(PORT + id);
    socket.unbind(address);
}