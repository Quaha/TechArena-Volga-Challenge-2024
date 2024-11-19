#include "graph.h"

fp penalty(fp c, fp M, fp F);
fp cost1(fp c, fp c_result, fp M, fp F);
fp cost2(fp c1, fp c2, fp c_result, fp M, fp F);
vector<int> block_dp(const graph &g, const vector<int> &edges);
// vector<int> cluster_dp(const graph &g, const vector<vector<int>> &block_edges);
