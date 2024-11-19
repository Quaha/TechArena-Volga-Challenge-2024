#pragma once
#include "graph.h"

vector<int> solve_edge_blocking_dp(const graph &g);
vector<int> solve_random_shuffle(const graph &g);
vector<int> solve_greedy_edgesort(const graph &g);