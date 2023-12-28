#pragma once

#include <iostream>
#include <zmq.hpp>
#include <string>
#include <sstream>
#include <unistd.h>
#include <unordered_map>
#include <optional>
#include <set>

#define PORT 4040

void send_message(zmq::socket_t& socket, const std::string& message);
std::string receive_message(zmq::socket_t& socket);
void connect(zmq::socket_t& socket, int id);
void disconnect(zmq::socket_t& socket, int id);
void bind(zmq::socket_t& socket, int id);
void unbind(zmq::socket_t& socket, int id);