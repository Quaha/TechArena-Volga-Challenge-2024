#include "includes.h"

#include "tester.h"
#include "checker.h"
#include "solver.h"

template <typename T> void print_vector(const vector<T> &v, int b, int e) {
    for (int i = b; i < e; ++i) {
        cout << v[i]+1 << ' ';
    }
}

void minus_one(vector<int> &p) {
    for (int &x : p) --x;
}

void run_tests() {
    int num_of_tests = 50;
    cout << "Run all tests" << endl;
    vector<graph> test_graph(num_of_tests);
    for (int test_i = 0; test_i < num_of_tests; ++test_i) {
        string path = "../test_samples/" + to_string(test_i + 1) + ".txt";
        ifstream ifs(path);
        test_graph[test_i] = read_input(ifs);
        ifs.close();
    }

    // vector<array<fp,2>> scores(200);
    fp total_score = 0.0;
    int tests_passed = 0;
#pragma omp parallel for reduction(+ : total_score)
    for (int test_i = 0; test_i < num_of_tests; ++test_i) {
        #pragma omp critical
        {
            cout << "Running on test " << (++tests_passed) << endl;
        }
        auto &g = test_graph[test_i];
        fp c_log_sum = 0.0;
        for (int i = 0; i < g.n; ++i) {
            c_log_sum += log(g.c[i]);
        }
        c_log_sum *= g.m;
        
        total_score += c_log_sum / log(calculate_score(solve_genetic(g,
                                                                     opt_max_pop_size,
                                                                     opt_mutations_per_iter,
                                                                     opt_selection_remain,
                                                                     opt_random_init_size), g));
        // scores[test_i] = {
        //     calculate_score(solve_random_shuffle(g), g),
        //     calculate_score(solve_edge_blocking_dp(g), g)
        // };
    }
    cout.precision(15);
    cout << "Total score: " << fixed << total_score << endl;
    // cout << setw(20) << "Method" << setw(15) << "Total score" << setw(12) << "Best count" << endl;
    // cout << setw(20) << "Random shuffle" << setw(15) << total_score[0] << setw(12) << best_score[0] << endl;
    // cout << setw(20) << "Edge-block DP" << setw(15) << total_score[1] << setw(12) << best_score[1] << endl;
}