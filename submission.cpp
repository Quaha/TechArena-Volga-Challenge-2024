#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>
#pragma once
using std::vector;
#define double long double
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
graph::graph(size_t n) : adj(n) {}
void graph::add_edge(int u, int v, double w) {
    adj[u].push_back(edges.size());
    adj[v].push_back(edges.size());
    edges.emplace_back(u, v, w);
}
using namespace std;
struct DSU {
    vector<int> root, sz;
    DSU(size_t n) : root(n), sz(n, 1) {
        iota(root.begin(), root.end(), 0);
    }
    int get(int x) {
        return x == root[x] ? x : root[x] = get(root[x]);
    }
    void merge(int x, int y) {
        x = get(x);
        y = get(y);
        if (x == y) return;
        if (sz[x] > sz[y]) swap(x,y);
        sz[y] += sz[x];
        root[x] = y;
    }
};
struct KahanSum {
    double s = 0.0;
    double e = 0.0;
    void add(double x) {
        double d = x - e;
        double t = s + d;
        e = (t - s) - d;
        s = t;
    }
};
double penalty(double c, double M, double F) {
    if (c <= M) return 0.0;
    return F * (c - M);
}
double cost1(double c, double c_result, double M, double F) {
    return c + c_result + penalty(c_result, M, F);
}
double cost2(double c1, double c2, double c_result, double M, double F) {
    return c1 + c2 + c_result + penalty(c_result, M, F);
}
double check(const vector<int> &p, const graph &g, vector<double> c,
             int n, int m, double M, double F) {
    KahanSum A;
    DSU dsu(n);
    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = dsu.get(u);
        v = dsu.get(v);
        if (u == v) {
            double c_result = w * c[u];
            A.add(cost1(c[u], c_result, M, F));
            c[u] = c_result;
        } else {
            double c1 = c[u];
            double c2 = c[v];
            double c_result = w * c1 * c2;
            A.add(cost2(c[u], c[v], c_result, M, F));
            dsu.merge(u,v);
            c[dsu.get(u)] = c_result;
        }
    }
    return log(A.s);
}
using namespace std;
template <typename T>
void print_vector(const vector<T> &v, int b, int e) {
    for (int i = b; i < e; ++i) {
        cout << v[i]+1 << ' ';
    }
}
void minus_one(vector<int> &p) {
    for (int &x : p) --x;
}
void run_tests() {
    int n,m; cin >> n >> m;
    double M, F; cin >> M >> F;
    vector<double> c(n);
    for (int i = 0; i < n; ++i) cin >> c[i];
    graph g(n);
    for (int i = 0; i < m; ++i) {
        int u,v;
        double s;
        cin >> u >> v >> s;
        --u, --v;
        g.add_edge(u, v, s);
    }
    double c_log_sum = 0.0;
    for (int i = 0; i < n; ++i) {
        c_log_sum += log(c[i]);
    }
    c_log_sum *= m;
    
    vector<int> p(m), p_max;
    iota(p.begin(), p.end(), 0);
    double max_score = 0.0;
    mt19937_64 rng{random_device{}()};
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() / 1000000000.0;
    };
    while (get_time_seconds() < 4.9) {
        shuffle(p.begin(), p.end(), rng);
        double score = c_log_sum / check(p, g, c, n, m, M, F);
        if (score > max_score) {
            p_max = p;
            max_score = score;
        }
    }
    print_vector(p_max, 0, m); cout << '\n';
    // cout << "Best score: " << max_score << '\n';
    // cout << "Permutation: "; print_vector(p_max, 0, m); cout << '\n';
}
using namespace std;
// #define FILE_INPUT
int main(int argc, char *argv[]) {
#ifdef FILE_INPUT
    freopen("../input.txt", "r", stdin);
#endif
    run_tests();
}
