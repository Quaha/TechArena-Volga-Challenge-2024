#include "param_search.h"
#include "graph.h"
#include "solver.h"
#include "checker.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <string>
using namespace std;

void param_search() {
    int num_of_tests = 8;
    vector<graph> test_graph(num_of_tests);
    for (int test_i = 0; test_i < num_of_tests; ++test_i) {
        string path = "../test_samples/" + to_string(test_i + 1) + ".txt";
        ifstream ifs(path);
        test_graph[test_i] = read_input(ifs);
        ifs.close();
    }

    cout << "Search for optimal parameters of GA" << endl;
    ofstream verbose_out("param_search.txt");

    fp max_total_score = numeric_limits<fp>::lowest();
    int optimal_max_pop_size = -1;
    int optimal_mutations_per_iter = -1;
    int optimal_selection_remain = -1;
    int optimal_random_init_size = -1;


    for (int max_pop_size = 256; max_pop_size <= 2048; max_pop_size += 256) {
        for (int mutations_per_iter = 16; mutations_per_iter <= 128; mutations_per_iter += 16) {
            for (int selection_remain = 128; selection_remain <= max_pop_size / 2; selection_remain += 64) {
                for (int random_init_size = 2048; random_init_size <= 10240; random_init_size += 2048) {
                    fp total_score = 0.0;
#pragma omp parallel for reduction(+ : total_score)
                    for (int test_i = 0; test_i < num_of_tests; ++test_i) {
                        auto &g = test_graph[test_i];
                        fp c_log_sum = 0.0;
                        for (int i = 0; i < g.n; ++i) {
                            c_log_sum += log(g.c[i]);
                        }
                        c_log_sum *= g.m;
                        fp score = numeric_limits<fp>::max();
                        for (int repeats = 0; repeats < 3; ++repeats) {
                            score = min(score, calculate_score(solve_genetic(g, max_pop_size,
                                                    mutations_per_iter, selection_remain), g));
                        }
                        total_score += c_log_sum / log(score);
                    }
                    string verb = "Total score for (" + to_string(max_pop_size) + ","
                                + to_string(mutations_per_iter) + ","
                                + to_string(selection_remain) + ","
                                + to_string(random_init_size) + "): " + to_string(total_score);
                    if (total_score > max_total_score) {
                        max_total_score = total_score;
                        optimal_max_pop_size = max_pop_size;
                        optimal_mutations_per_iter = mutations_per_iter;
                        optimal_selection_remain = selection_remain;
                        optimal_random_init_size = random_init_size;
                    }
                    cout << verb << endl;
                    verbose_out << verb << endl;
                }
            }
        }
    }

    string verb =  "Optimal parameters: (" + to_string(optimal_max_pop_size) + ","
                    + to_string(optimal_mutations_per_iter) + ","
                    + to_string(optimal_selection_remain) + ","
                    + to_string(optimal_random_init_size) +"): " + to_string(max_total_score);
    cout << verb << endl;
    verbose_out << verb << endl;

    verbose_out.close();
}