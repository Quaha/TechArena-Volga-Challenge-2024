#include <numeric>
#include <cmath>
#include "checker.h"
#include "util.h"
using namespace std;

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

double check(const vector<int> &p, const graph &g) {
    KahanSum A;
    DSU dsu(g.n);
    vector<double> c = g.c;

    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = dsu.get(u);
        v = dsu.get(v);
        if (u == v) {
            double c_result = w * c[u];
            A.add(cost1(c[u], c_result, g.M, g.F));
            c[u] = c_result;
        } else {
            double c1 = c[u];
            double c2 = c[v];
            double c_result = w * c1 * c2;
            A.add(cost2(c[u], c[v], c_result, g.M, g.F));
            dsu.merge(u,v);
            c[dsu.get(u)] = c_result;
        }
    }

    return A.s;
}