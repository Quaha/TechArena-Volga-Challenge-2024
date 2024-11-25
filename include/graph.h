#pragma once

#include "includes.h"

struct edge {
    int u,v;
    fp w;
    edge(int u, int v, fp w) : u(u), v(v), w(w) {};
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
    vector<int> root, sz, upd;
    DSU();
    DSU(int n);
    void init(int n);
    int get(int x);
    void merge(int x, int y);
};

vector<vector<int>> get_connected_components(const graph &g);