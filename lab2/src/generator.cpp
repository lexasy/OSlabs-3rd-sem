#include "generator.hpp"

std::vector<int> generate_tests(int quantity) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 4096);
    std::vector<int> vec;
    for (int i = 0; i < quantity; i++) {
        vec.push_back(distrib(gen));
    }
    return vec;
}