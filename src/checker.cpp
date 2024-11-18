#include <algorithm>
#include <numeric>
#include <cmath>
#include "checker.h"
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
    double c_log_sum = 0.0;
    double A = 0.0;
    DSU dsu(n);

    for (int i = 0; i < n; ++i) {
        c_log_sum += log(c[i]);
    }

    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = dsu.get(u);
        v = dsu.get(v);
        if (u == v) {
            double c_result = w * c[u];
            A += cost1(c[u], c_result, M, F);
            c[u] = c_result;
        } else {
            double c1 = c[u];
            double c2 = c[v];
            double c_result = w * c1 * c2;
            A += cost2(c[u], c[v], c_result, M, F);
            dsu.merge(u,v);
            c[dsu.get(u)] = c_result;
        }
    }

    return m * (c_log_sum / log(A));
}