#include "includes.h"
#include "param_search.h"
#include "graph.h"
#include "solver.h"
#include "checker.h"
using namespace std;

void param_search() {
    int num_of_tests = 64;
    vector<graph> test_graph(num_of_tests);
    for (int test_i = 0; test_i < num_of_tests; ++test_i) {
        string path = "../test_samples/" + to_string(test_i + 1) + ".txt";
        ifstream ifs(path);
        test_graph[test_i] = read_input(ifs);
        ifs.close();
    }

    cout << "Search for optimal parameters of GA" << endl;
    ofstream verbose_out("param_search_output.txt");

    fp max_total_score = numeric_limits<fp>::lowest();
    int optimal_max_pop_size = -1;
    int optimal_mutations_per_iter = -1;
    int optimal_selection_remain = -1;
    int optimal_random_init_size = -1;

    vector<array<int, 4>> params;
    vector<fp> best_score;
    vector<fp> avg_score;

    // int max_pop_size = 768;
    // int mutations_per_iter = 96;
    // int selection_remain = 128;
    const int param_len = 40;
    const int score_len = 20;
    cout.precision(12);
    verbose_out.precision(12);
    cout << setw(param_len) << "Parameters" << setw(score_len) << "Best score" << setw(score_len) << "Average score" << '\n';
    verbose_out << setw(param_len) << "Parameters" << setw(score_len) << "Best score" << setw(score_len) << "Average score" << '\n';
    for (int max_pop_size = 256; max_pop_size <= 2048; max_pop_size += 256) {
        for (int mutations_per_iter = 16; mutations_per_iter <= 128; mutations_per_iter += 16) {
            for (int selection_remain = 128; selection_remain <= max_pop_size / 2; selection_remain += 128) {
                for (int random_init_size = 20480; random_init_size <= 204800; random_init_size += 20480) {
                    fp best_total_score = 0.0, avg_total_score = 0.0;
                    for (int repeats = 0; repeats < 3; ++repeats) {
                        fp total_score = 0.0;
#pragma omp parallel for reduction(+ : total_score)
                        for (int test_i = 0; test_i < num_of_tests; ++test_i) {
                            auto &g = test_graph[test_i];
                            fp c_log_sum = 0.0;
                            for (int i = 0; i < g.n; ++i) {
                                c_log_sum += log(g.c[i]);
                            }
                            c_log_sum *= g.m;
                            fp score = calculate_score(solve_genetic(g,
                                                       max_pop_size,
                                                       mutations_per_iter,
                                                       selection_remain,
                                                       random_init_size,
                                                       def_best_selection_rate,
                                                       def_crossover_rate,
                                                       def_repeats), g);
                            total_score += c_log_sum / log(score);
                        }
                        avg_total_score += total_score;
                        best_total_score = max(best_total_score, total_score);
                    }
                    avg_total_score /= 3.0;
                    string verb = "Total score for (" + to_string(max_pop_size) + ","
                                + to_string(mutations_per_iter) + ","
                                + to_string(selection_remain) + ","
                                + to_string(random_init_size) + "):";
                    params.push_back({max_pop_size, mutations_per_iter, selection_remain, random_init_size});
                    best_score.push_back(best_total_score);
                    avg_score.push_back(avg_total_score);

                    cout << setw(param_len) << verb << setw(score_len) << best_total_score << setw(score_len) << avg_total_score << '\n';
                    verbose_out << setw(param_len) << verb << setw(score_len) << best_total_score << setw(score_len) << avg_total_score << '\n';
                }
            }
        }
    }

    vector<int> idx(params.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&](int i, int j) {
        return best_score[i] > best_score[j];
    });
    cout << "Top-10 parameters by best score:\n";
    verbose_out << "Top-10 parameters by best score:'\n";
    cout << setw(param_len) << "Parameters" << setw(score_len) << "Best score" << setw(score_len) << "Average score" << '\n';
    verbose_out << setw(param_len) << "Parameters" << setw(score_len) << "Best score" << setw(score_len) << "Average score" << '\n';
    for (int i = 0; i < 10; ++i) {
        string verb = "(" + to_string(params[idx[i]][0]) + ","
                          + to_string(params[idx[i]][1]) + ","
                          + to_string(params[idx[i]][2]) + ","
                          + to_string(params[idx[i]][3]) + "):";
        cout << setw(param_len) << verb << setw(score_len) << best_score[idx[i]] << setw(score_len) << avg_score[idx[i]] << '\n';
        verbose_out << setw(param_len) << verb << setw(score_len) << best_score[idx[i]] << setw(score_len) << avg_score[idx[i]] << '\n';
    }
    sort(idx.begin(), idx.end(), [&](int i, int j) {
        return avg_score[i] > avg_score[j];
    });
    cout << "Top-10 parameters by average score:\n";
    verbose_out << "Top-10 parameters by average score:'\n";
    cout << setw(param_len) << "Parameters" << setw(14) << "Best score" << setw(14) << "Average score" << '\n';
    verbose_out << setw(param_len) << "Parameters" << setw(14) << "Best score" << setw(14) << "Average score" << '\n';
    for (int i = 0; i < 10; ++i) {
        string verb = "(" + to_string(params[idx[i]][0]) + ","
                          + to_string(params[idx[i]][1]) + ","
                          + to_string(params[idx[i]][2]) + ","
                          + to_string(params[idx[i]][3]) + "):";
        cout << setw(param_len) << verb << setw(score_len) << best_score[idx[i]] << setw(score_len) << avg_score[idx[i]] << '\n';
        verbose_out << setw(param_len) << verb << setw(score_len) << best_score[idx[i]] << setw(score_len) << avg_score[idx[i]] << '\n';
    }

    // string verb =  "Optimal parameters: (" + to_string(optimal_max_pop_size) + ","
    //                 + to_string(optimal_mutations_per_iter) + ","
    //                 + to_string(optimal_selection_remain) + ","
    //                 + to_string(optimal_random_init_size) +"): " + to_string(max_total_score);
    // cout << verb << endl;
    // verbose_out << verb << endl;

    verbose_out.close();
}