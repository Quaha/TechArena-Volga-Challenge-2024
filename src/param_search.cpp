#include "includes.h"
#include "param_search.h"
#include "graph.h"
#include "solver.h"
#include "checker.h"
using namespace std;

void param_search() {
    int num_of_tests = 24;
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
    int optimal_repeats = -1;

    vector<array<int, 5>> params;
    vector<fp> best_score;
    vector<fp> avg_score;

    const int param_len = 40;
    const int score_len = 24;
    cout.precision(16);
    verbose_out.precision(16);
    cout << setw(param_len) << "Parameters" << setw(score_len) << "Best score" << setw(score_len) << "Average score" << '\n';
    verbose_out << setw(param_len) << "Parameters" << setw(score_len) << "Best score" << setw(score_len) << "Average score" << '\n';
    
    vector<int> max_pop_size_list{32, 64, 96, 128};
    vector<int> mutations_per_iter_list{8, 16, 32, 48, 64};
    vector<int> selection_remain_list{8, 16, 32, 64};
    vector<int> random_init_size_list{64, 128, 256, 512};
    vector<int> repeats_list{4, 8, 10, 12};

    for (int max_pop_size : max_pop_size_list) {
        for (int mutations_per_iter : mutations_per_iter_list) {
            for (int selection_remain : selection_remain_list) {
                for (int random_init_size : random_init_size_list) {
                    for (int repeats : repeats_list) {
                        if (selection_remain > max_pop_size / 2 || mutations_per_iter >= max_pop_size / 4) continue;
                        fp best_total_score = 0.0, avg_total_score = 0.0;
                        for (int rep = 0; rep < 3; ++rep) {
                            fp total_score = 0.0;
    #pragma omp parallel for reduction(+ : total_score)
                            for (int test_i = 0; test_i < num_of_tests; ++test_i) {
                                auto &g = test_graph[test_i];
                                fp c_log_sum = 0.0;
                                for (int i = 0; i < g.n; ++i) {
                                    c_log_sum += log(g.c[i]);
                                }
                                c_log_sum *= g.m;
                                fp score = calculate_score_precise(solve_genetic(g,
                                                        max_pop_size,
                                                        mutations_per_iter,
                                                        selection_remain,
                                                        random_init_size,
                                                        def_best_selection_rate,
                                                        def_crossover_rate,
                                                        repeats), g);
                                total_score += c_log_sum / score;
                            }
                            avg_total_score += total_score;
                            best_total_score = max(best_total_score, total_score);
                        }
                        avg_total_score /= 3.0;
                        string verb = "Total score for (" + to_string(max_pop_size) + ","
                                    + to_string(mutations_per_iter) + ","
                                    + to_string(selection_remain) + ","
                                    + to_string(random_init_size) + ","
                                    + to_string(repeats) + "):";
                        params.push_back({max_pop_size, mutations_per_iter, selection_remain, random_init_size, repeats});
                        best_score.push_back(best_total_score);
                        avg_score.push_back(avg_total_score);

                        cout << setw(param_len) << verb << setw(score_len) << best_total_score << setw(score_len) << avg_total_score << '\n';
                        verbose_out << setw(param_len) << verb << setw(score_len) << best_total_score << setw(score_len) << avg_total_score << '\n';
                    }
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
                          + to_string(params[idx[i]][3]) + ","
                          + to_string(params[idx[i]][4]) + "):";
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
                          + to_string(params[idx[i]][3]) + ","
                          + to_string(params[idx[i]][4]) + "):";
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