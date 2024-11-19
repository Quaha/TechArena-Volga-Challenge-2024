#pragma once
#include <algorithm>
#include <vector>
#include <cstddef>
using std::vector;

using fp = long double;

struct edge {
    int u,v;
    fp w;
};

struct graph {
    vector<vector<int>> adj;
    vector<edge> edges;
    vector<fp> c;
    int n,m;
    fp M,F;
    graph();
    graph(int n);
    void add_edge(int u, int v, fp w);
};

struct DSU {
    vector<int> root, sz;
    DSU();
    DSU(int n);
    void init(int n);
    int get(int x);
    void merge(int x, int y);
};