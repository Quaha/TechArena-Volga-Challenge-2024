#include "includes.h"

#include "graph.h"

fp penalty(fp c, fp M, fp F) {
    if (c <= M) return 0.0;
    return F * (c - M);
}

fp cost1(fp c, fp c_result, fp M, fp F) {
    return c + c_result + penalty(c_result, M, F);
}

fp cost2(fp c1, fp c2, fp c_result, fp M, fp F) {
    return c1 + c2 + c_result + penalty(c_result, M, F);
}

struct KahanSum {
    fp s = 0.0;
    fp e = 0.0;
    void add(fp x) {
        fp d = x - e;
        fp t = s + d;
        e = (t - s) - d;
        s = t;
    }
};

fp calculate_score(const vector<int> &p, const graph &g) {
    KahanSum A;
    DSU dsu(g.n);
    vector<fp> c = g.c;

    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = dsu.get(u);
        v = dsu.get(v);
        if (u == v) {
            fp c_result = w * c[u];
            A.add(cost1(c[u], c_result, g.M, g.F));
            c[u] = c_result;
        } else {
            fp c1 = c[u];
            fp c2 = c[v];
            fp c_result = w * c1 * c2;
            A.add(cost2(c[u], c[v], c_result, g.M, g.F));
            dsu.merge(u,v);
            c[dsu.get(u)] = c_result;
        }
    }

    return A.s;
}