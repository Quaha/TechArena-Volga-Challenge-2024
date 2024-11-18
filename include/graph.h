#pragma once
#include <vector>
#include <cstddef>
using std::vector;

// #define double long double

struct edge {
    int u,v;
    double w;
};

struct graph {
    vector<vector<int>> adj;
    vector<edge> edges;
    graph(size_t n);
    void add_edge(int u, int v, double w);
};