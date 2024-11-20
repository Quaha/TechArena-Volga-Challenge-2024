#include "graph.h"

// inline fp penalty(fp c, fp M, fp F);
// inline fp cost1(fp c, fp c_result, fp M, fp F);
// inline fp cost2(fp c1, fp c2, fp c_result, fp M, fp F);

inline fp penalty(fp c, fp M, fp F) {
    if (c <= M) return 0.0;
    return F * (c - M);
}

inline fp cost1(fp c, fp c_result, fp M, fp F) {
    return c + c_result + penalty(c_result, M, F);
}

inline fp cost2(fp c1, fp c2, fp c_result, fp M, fp F) {
    return c1 + c2 + c_result + penalty(c_result, M, F);
}

vector<int> block_dp(const graph &g, const vector<int> &edges);
