#pragma once
#include "graph.h"

void solve();
vector<int> solve_random_shuffle(const graph &g);
vector<int> solve_genetic(const graph &g,
                          int max_pop_size,
                          int mutations_per_iter,
                          int selection_remain,
                          int random_init_size,
                          int best_selection_rate,
                          int crossover_rate,
                          int repeats);
vector<int> solve_genetic(const graph &g);
vector<int> solve_dp(const graph &g);