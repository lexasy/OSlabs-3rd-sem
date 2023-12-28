#include "../include/topology.hpp"

void Topology::insert(int id, int parent_id) {
    if (parent_id == -1) {
        std::list<int> new_list;
        new_list.push_back(id);
        data.push_back(new_list);
    }
    int list_id = find(parent_id);
    if (list_id == -1) {
        throw std::runtime_error("Wrong parent id");
    }
    std::list<std::list<int>>::iterator external = data.begin();
    std::advance(external, list_id);
    for (std::list<int>::iterator internal = external->begin(); internal != external->end(); internal++) {
        if (*internal == parent_id) {
            external->insert(++internal, id);
            break;
        }
    }
}

int Topology::find(int id) {
    int curr_id = 0;
    for (std::list<std::list<int>>::iterator external = data.begin(); external != data.end(); external++) {
        for (std::list<int>::iterator internal = external->begin(); internal != external->end(); internal++) {
            if (*internal == id) {
                return curr_id;
            }
        }
        curr_id++;
    }
    return -1;
}

void Topology::erase(int id) {
    int list_id = find(id);
    if (list_id == -1) {
        throw std::runtime_error("Wrong id");
    }
    std::list<std::list<int>>::iterator external = data.begin();
    std::advance(external, list_id);
    for (std::list<int>::iterator internal = external->begin(); internal != external->end(); internal++) {
        if (*internal == id) {
            external->erase(internal, external->end());
            if (external->empty()) {
                data.erase(external);
            }
            break;
        }
    }
}

