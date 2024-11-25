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

#ifdef KAHAN_SUM
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
#endif

#if defined(OPTIMIZED_DSU)

int root[200], sz[200], upd[200];
void static_dsu_init(int n) {
    for (int i = 0; i < n; ++i) {
        root[i] = i;
        sz[i] = 1;
    }
}
int static_dsu_get(int x) {
    int ui = 0;
    while (x != root[x]) {
        upd[ui++] = x;
        x = root[x];
    }
    for (int i = 0; i < ui; ++i) {
        root[upd[i]] = x;
    }
    return x;
}
int static_dsu_merge(int x, int y) {
    x = static_dsu_get(x);
    y = static_dsu_get(y);
    if (x == y) return x;
    if (sz[x] > sz[y]) swap(x,y);
    sz[y] += sz[x];
    root[x] = y;
    return y;
}

fp score_c[200];
fp calculate_score(const vector<int> &p, const graph &g) {
    static_dsu_init(g.n);
    for (int i = 0; i < g.n; ++i) score_c[i] = g.c[i];

#ifdef KAHAN_SUM
    KahanSum A;
#else
    fp A = 0.0;
#endif
    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = static_dsu_get(u);
        v = static_dsu_get(v);
        if (u == v) {
            fp c = score_c[u];
            fp c_result = w * c;
        #ifdef KAHAN_SUM
            A.add(cost1(c, c_result, g.M, g.F));
        #else
            A += cost1(c, c_result, g.M, g.F);
        #endif
            score_c[u] = c_result;
        } else {
            fp c1 = score_c[u];
            fp c2 = score_c[v];
            fp c_result = w * c1 * c2;
        #ifdef KAHAN_SUM
            A.add(cost2(c1, c2, c_result, g.M, g.F));
        #else
            A += cost2(c1, c2, c_result, g.M, g.F);
        #endif
            
            score_c[static_dsu_merge(u,v)] = c_result;
        }
    }

#ifdef KAHAN_SUM
    return A.s;
#else
    return A;
#endif
}
#else

fp calculate_score(const vector<int> &p, const graph &g) {
    DSU score_dsu(g.n);
    vector<fp> score_c = g.c;

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
#endif

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