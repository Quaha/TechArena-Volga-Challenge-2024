#include "includes.h"

#include "graph.h"

graph::graph() {}

graph::graph(int n) : n(n), adj(n), c(n) {}

void graph::add_edge(int u, int v, fp w) {
    adj[u].push_back(edges.size());
    adj[v].push_back(edges.size());
    edges.emplace_back(u, v, w);
}

DSU::DSU() {}

DSU::DSU(int n) {
    init(n);
}

void DSU::init(int n) {
    root.resize(n);
    iota(root.begin(), root.end(), 0);
    sz.assign(n, 1);
}

int DSU::get(int x) {
    return x == root[x] ? x : root[x] = get(root[x]);
}

void DSU::merge(int x, int y) {
    x = get(x);
    y = get(y);
    if (x == y) return;
    if (sz[x] > sz[y]) swap(x,y);
    sz[y] += sz[x];
    root[x] = y;
}