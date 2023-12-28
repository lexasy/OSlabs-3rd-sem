#pragma once

#include <iostream>
#include <list>

class Topology {
    public: 
        void insert(int id, int parent_id);
        void erase(int id);
        int find(const int node);
    private:
	    std::list<std::list<int>> data;
};