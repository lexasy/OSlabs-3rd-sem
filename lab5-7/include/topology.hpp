#pragma once

#include <iostream>
#include <list>

class Topology {
private:
    using listType = std::list<std::list<int>>;

    listType list;
public:
    Topology() : list() {}

    void insert(int id, int parentId);
    int find(int id);
    void erase(int id);
    int getFirstId(int listId);
};