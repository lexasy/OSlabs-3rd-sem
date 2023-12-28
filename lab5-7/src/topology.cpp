#include "../include/topology.hpp"

void Topology::insert(int id, int parentId) {
    if (parentId == -1) {
        std::list<int> newList;
        newList.push_back(id);
        list.push_back(newList);
        return;
    }
    int listId = find(parentId);
    if (listId == -1) {
        throw std::runtime_error("Wrong parent id");
    }
    auto it1 = list.begin();
    std::advance(it1, listId);
    for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
        if (*it2 == parentId) {
            it1->insert(++it2, id);
            return;
        }
    }
}

int Topology::find(int id) {
    int curListId = 0;
    for (auto it1 = list.begin(); it1 != list.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (*it2 == id) {
                return curListId;
            }
        }
        ++curListId;
    }
    return -1;
}

void Topology::erase(int id) {
    int listId = find(id);
    if (listId == -1) {
        throw std::runtime_error("Wrong id");
    }
    auto it1 = list.begin();
    std::advance(it1, listId);
    for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
        if (*it2 == id) {
            it1->erase(it2, it1->end());
            if (it1->empty()) {
                list.erase(it1);
            }
            return;
        }
    }
}

int Topology::getFirstId(int listId) {
    auto it1 = list.begin();
    std::advance(it1, listId);
    if (it1->begin() == it1->end()) {
        return -1;
    }
    return *(it1->begin());
}