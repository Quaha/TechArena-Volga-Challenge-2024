#pragma once
#include "graph.h"

vector<int> solve_random_shuffle(const graph &g);
vector<int> solve_greedy_edgesort(const graph &g);
vector<int> solve_genetic(const graph &g,
                          int max_pop_size = 1000,
                          int mutations_per_iter = 20,
                          int selection_remain = 100);
vector<int> solve_dp(const graph &g);