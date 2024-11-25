#pragma once
#include "graph.h"

fp penalty(fp c, fp M, fp F);
fp cost1(fp c, fp c_result, fp M, fp F);
fp cost2(fp c1, fp c2, fp c_result, fp M, fp F);

fp calculate_score(const vector<int> &p, const graph &g);
fp calculate_score_precise(const vector<int> &p, const graph &g);