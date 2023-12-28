#pragma once

#include <iostream>
#include <zmq.hpp>
#include <string>
#include <unistd.h>
#include <sstream>
#include <set>
#include <unordered_map>
#include <optional>

constexpr int MAIN_PORT = 4040;

void sendMessage(zmq::socket_t& socket, const std::string& msg);
std::string receiveMessage(zmq::socket_t& socket);
void connect(zmq::socket_t& socket, int id);
void disconnect(zmq::socket_t& socket, int id);
void bind(zmq::socket_t& socket, int id);
void unbind(zmq::socket_t& socket, int id);