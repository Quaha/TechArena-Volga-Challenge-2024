#include <functional>
#include <numeric>
#include <utility>
#include "graph.h"
using namespace std;

graph::graph() {}
graph::graph(int n) : n(n), adj(n), c(n) {}
void graph::add_edge(int u, int v, fp w) {
    adj[u].push_back(edges.size());
    adj[v].push_back(edges.size());
    edges.emplace_back(u, v, w);
}

graph read_input(istream &ist) {
    int n; ist >> n;
    graph g(n);
    ist >> g.m >> g.M >> g.F;
    for (int i = 0; i < n; ++i) ist >> g.c[i];

    for (int i = 0; i < g.m; ++i) {
        int u,v;
        fp s;
        ist >> u >> v >> s;
        --u, --v;
        g.add_edge(u, v, s);
    }
    return g;
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

vector<vector<int>> get_connected_components(const graph &g) {
    vector<vector<int>> comp;
    vector<char> was(g.n);
    int comp_num = 0;
    function<void(int)> dfs = [&](int u) {
        was[u] = 1;
        comp.back().push_back(u);
        for (int ei : g.adj[u]) {
            auto &[x, y, w] = g.edges[ei];
            int v = x ^ y ^ u;
            if (was[v]) continue;
            dfs(v);
        }
    };
    for (int i = 0; i < g.n; ++i) {
        if (!was[i]) {
            comp.emplace_back();
            dfs(i);
            ++comp_num;
        }
    }
    return comp;
}