#include "includes.h"

#include "block_dp.h"
#include "checker.h"
using namespace std;
const int MAX_M = 21;

#ifndef RUN_TESTS
DSU mask_dsu[1 << MAX_M];
vector<fp> mask_c[1 << MAX_M];
fp dp[1 << MAX_M];
int dp_arg[1 << MAX_M];
#endif

vector<int> block_dp(const graph &g, const vector<int> &edges) {
#ifdef RUN_TESTS
    vector<DSU           > mask_dsu(1 << MAX_M);
    vector<vector<fp>> mask_c(1 << MAX_M);
    vector<fp        > dp(1 << MAX_M);
    vector<int           > dp_arg(1 << MAX_M);
#endif
    int m = edges.size();
#ifdef MINGW
    mask_dsu[0].init(g.n);
    mask_c[0] = g.c;
    for (unsigned int mask = 1; mask < (1 << m); ++mask) {
        dp[mask] = numeric_limits<fp>::max();
        int bit = countr_zero(mask);
        mask_dsu[mask] = mask_dsu[mask ^ (1 << bit)];
        mask_c[mask] = mask_c[mask ^ (1 << bit)];
        auto [u, v, w] = g.edges[edges[bit]];
        u = mask_dsu[mask].get(u);
        v = mask_dsu[mask].get(v);
        if (u == v) {
            mask_c[mask][u] *= w;
        } else {
            fp c1 = mask_c[mask][u];
            fp c2 = mask_c[mask][v];
            fp c_result = w * c1 * c2;
            mask_dsu[mask].merge(u, v);
            u = mask_dsu[mask].get(u);
            mask_c[mask][u] = c_result;
        }
    }
    for (unsigned int mask = 0; mask < (1 << m) - 1; ++mask) {
        for (int bit = 0; bit < m; ++bit) {
            if (mask & (1 << bit)) continue;
            fp cost = 0.0;
            auto [u, v, w] = g.edges[edges[bit]];
            u = mask_dsu[mask].get(u);
            v = mask_dsu[mask].get(v);
            int next_mask = mask ^ (1 << bit);
            if (u == v) {
                fp c = mask_c[mask][u];
                fp c_result = c * w;
                cost = cost1(c, c_result, g.M, g.F);
            } else {
                fp c1 = mask_c[mask][u];
                fp c2 = mask_c[mask][v];
                fp c_result = w * c1 * c2;
                cost = cost2(c1, c2, c_result, g.M, g.F);
            }
            if (cost + dp[mask] < dp[next_mask]) {
                dp_arg[next_mask] = bit;
                dp[next_mask] = cost + dp[mask];
            }
        }
    }
    vector<int> ans;
    for (int mask = (1 << m) - 1; mask > 0; mask ^= (1 << dp_arg[mask])) {
        ans.push_back(edges[dp_arg[mask]]);
    }
    reverse(ans.begin(), ans.end());
#endif
#ifndef MINGW
    vector<int> ans;
    for (int i = 1; i <= m; i++) {
        ans.push_back(i);
    }
#endif
    return ans;
}

