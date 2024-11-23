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
        s += x;
        fp d = x - e;
        fp t = s + d;
        e = (t - s) - d;
        s = t;
    }
};

#ifndef PARAM_SEARCH
DSU score_dsu(200);
fp score_c[200];
#endif

fp calculate_score(const vector<int> &p, const graph &g) {
#ifdef PARAM_SEARCH
    DSU score_dsu(g.n);
    vector<fp> score_c = g.c;
#else
    score_dsu.init(g.n);
    for (int i = 0; i < g.n; ++i) score_c[i] = g.c[i];
#endif

    KahanSum A;
    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = score_dsu.get(u);
        v = score_dsu.get(v);
        if (u == v) {
            fp c_result = w * score_c[u];
            A.add(cost1(score_c[u], c_result, g.M, g.F));
            score_c[u] = c_result;
        } else {
            fp c1 = score_c[u];
            fp c2 = score_c[v];
            fp c_result = w * c1 * c2;
            A.add(cost2(score_c[u], score_c[v], c_result, g.M, g.F));
            score_dsu.merge(u,v);
            score_c[score_dsu.get(u)] = c_result;
        }
    }

    return A.s;
}

fp calculate_score_precise(const vector<int> &p, const graph &g) {
    fp exp_val_arr[1 << 16]{};
    fp *exp_val = exp_val_arr + (1 << 15);
    auto add = [&](fp x) {
        int ex;
        fp mant = frexp(x, &ex);
        exp_val[ex] += mant;
    };
    auto penalty_precise = [&](fp c, fp M, fp F) {
        if (c <= M) return;
        add(F * c);
        add(- F * M);
    };
    auto cost1_precise = [&](fp c, fp c_result, fp M, fp F) {
        add(c);
        add(c_result);
        penalty_precise(c_result, M, F);
    };
    auto cost2_precise = [&](fp c1, fp c2, fp c_result, fp M, fp F) {
        add(c1);
        add(c2);
        add(c_result);
        penalty_precise(c_result, M, F);
    };
    auto total_score = [&]() {
        int max_exp = 0;
        int min_exp = 0;
        for (int i = -(1<<14); i <= (1<<14); ++i) {
            if (exp_val[i] != fp(0.0)) {
                min_exp = min(min_exp, i);
                max_exp = max(max_exp, i);
            }
        }
        fp mant = 0.0;
        for (int i = 0; i <= max_exp; ++i) {
            mant = ldexp(mant, -1);
            mant += exp_val[i];
        }
        return log2(mant) + max_exp;
    };

    DSU score_dsu(g.n);
    vector<fp> score_c = g.c;

    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = score_dsu.get(u);
        v = score_dsu.get(v);
        if (u == v) {
            fp c_result = w * score_c[u];
            cost1_precise(score_c[u], c_result, g.M, g.F);
            score_c[u] = c_result;
        } else {
            fp c1 = score_c[u];
            fp c2 = score_c[v];
            fp c_result = w * c1 * c2;
            cost2_precise(score_c[u], score_c[v], c_result, g.M, g.F);
            score_dsu.merge(u,v);
            score_c[score_dsu.get(u)] = c_result;
        }
    }

    return total_score();
}