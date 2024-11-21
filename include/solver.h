#pragma once
#include "graph.h"

void solve();
vector<int> solve_random_shuffle(const graph &g);
vector<int> solve_greedy_edgesort(const graph &g);
vector<int> solve_genetic(const graph &g,
                          int max_pop_size = 768,
                          int mutations_per_iter = 96,
                          int selection_remain = 128,
                          int random_init_size = 10240);
vector<int> solve_dp(const graph &g);