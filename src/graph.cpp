#include "graph.h"

graph::graph(size_t n) : adj(n) {}
void graph::add_edge(int u, int v, double w) {
    adj[u].push_back(edges.size());
    adj[v].push_back(edges.size());
    edges.emplace_back(u, v, w);
}