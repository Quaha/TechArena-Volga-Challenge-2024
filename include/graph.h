#pragma once
#include <algorithm>
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
    vector<double> c;
    int n,m;
    double M,F;
    graph(int n);
    void add_edge(int u, int v, double w);
};

struct DSU {
    vector<int> root, sz;
    DSU();
    DSU(int n);
    void init(int n);
    int get(int x);
    void merge(int x, int y);
};