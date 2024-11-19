#include "graph.h"

double penalty(double c, double M, double F);
double cost1(double c, double c_result, double M, double F);
double cost2(double c1, double c2, double c_result, double M, double F);
vector<int> block_dp(const graph &g, const vector<int> &edges);
// vector<int> cluster_dp(const graph &g, const vector<vector<int>> &block_edges);
