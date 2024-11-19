#include "util.h"
using namespace std;

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

DSU mask_dsu[1 << 16];
vector<double> mask_c[1 << 16];
double dp[1 << 16];
int dp_arg[1 << 16];

vector<int> block_dp(const graph &g, const vector<int> &edges) {
    int m = edges.size();
    mask_dsu[0].init(g.n);
    mask_c[0] = g.c;
    for (int mask = 1; mask < (1 << m); ++mask) {
        dp[mask] = 1e38;
        int bit = __builtin_ctz(mask);
        mask_dsu[mask] = mask_dsu[mask ^ (1 << bit)];
        mask_c[mask] = mask_c[mask ^ (1 << bit)];
        auto [u, v, w] = g.edges[edges[bit]];
        u = mask_dsu[mask].get(u);
        v = mask_dsu[mask].get(v);
        if (u == v) {
            mask_c[mask][u] *= w;
        } else {
            double c1 = mask_c[mask][u];
            double c2 = mask_c[mask][v];
            double c_result = w * c1 * c2;
            mask_dsu[mask].merge(u, v);
            u = mask_dsu[mask].get(u);
            mask_c[mask][u] = c_result;
        }
    }
    for (int mask = 0; mask < (1 << m) - 1; ++mask) {
        for (int bit = 0; bit < m; ++bit) {
            if (mask & (1 << bit)) continue;
            double cost = 0.0;
            auto [u, v, w] = g.edges[edges[bit]];
            u = mask_dsu[mask].get(u);
            v = mask_dsu[mask].get(v);
            int next_mask = mask ^ (1 << bit);
            if (u == v) {
                double c = mask_c[mask][u];
                double c_result = c * w;
                cost = cost1(c, c_result, g.M, g.F);
            } else {
                double c1 = mask_c[mask][u];
                double c2 = mask_c[mask][v];
                double c_result = w * c1 * c2;
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
    return ans;
}

