#pragma once
#include <algorithm>
#include <fstream>
#include <limits>
#include <cstddef>
#include <vector>
using std::vector;
using std::istream;

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

graph read_input(istream &ist);

struct DSU {
    vector<int> root, sz;
    DSU();
    DSU(int n);
    void init(int n);
    int get(int x);
    void merge(int x, int y);
};

vector<vector<int>> get_connected_components(const graph &g);