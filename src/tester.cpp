#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>
#include <cassert>
#include <chrono>
#include <iomanip>
#include "tester.h"
#include "checker.h"
#include "solver.h"
using namespace std;

template <typename T>
void print_vector(const vector<T> &v, int b, int e) {
    for (int i = b; i < e; ++i) {
        cout << v[i]+1 << ' ';
    }
}

void minus_one(vector<int> &p) {
    for (int &x : p) --x;
}

graph read_input(istream &ist) {
    int n; ist >> n;
    graph g(n);
    ist >> g.m >> g.M >> g.F;
    for (int i = 0; i < n; ++i) ist >> g.c[i];

    for (int i = 0; i < g.m; ++i) {
        int u,v;
        fp s;
        ist >> u >> v >> s;
        --u, --v;
        g.add_edge(u, v, s);
    }
    return g;
}

void run_tests() {
    // int num_of_tests = 1;
#ifdef RUN_TESTS
    vector<graph> test_graph(200);
    for (int test_i = 0; test_i < 200; ++test_i) {
        string path = "../test_samples/" + to_string(test_i + 1) + ".txt";
        ifstream ifs(path);
        test_graph[test_i] = read_input(ifs);
        ifs.close();
    }

    vector<array<fp,2>> scores(200);
#pragma omp parallel for
    for (int test_i = 0; test_i < 200; ++test_i) {
        if ((test_i + 1) % 5 == 0) {
            string out = "Running on test " + to_string(test_i + 1);
            cout << out << endl;
        }
        auto &g = test_graph[test_i];
        scores[test_i] = {
            calculate_score(solve_random_shuffle(g), g),
            calculate_score(solve_edge_blocking_dp(g), g)
        };
    }
    for (int test_i = 0; test_i < 200; ++test_i) {
        cout << scores[test_i][0] << ' ' << scores[test_i][1] << '\n';
    }
    fp total_score[2]{};
    int best_score[2]{};
    for (int test_i = 0; test_i < 200; ++test_i) {
        fp c_log_sum = 0.0;
        auto &g = test_graph[test_i];
        for (int i = 0; i < g.n; ++i) {
            c_log_sum += log(g.c[i]);
        }
        c_log_sum *= g.m;
        for (int method_i = 0; method_i < 2; ++method_i) {
            total_score[method_i] += c_log_sum / log(scores[test_i][method_i]);
        }
        ++best_score[min_element(scores[test_i].begin(), scores[test_i].end()) - scores[test_i].begin()];
    }
    cout << setw(20) << "Method" << setw(15) << "Total score" << setw(12) << "Best count" << endl;
    cout << setw(20) << "Random shuffle" << setw(15) << total_score[0] << setw(12) << best_score[0] << endl;
    cout << setw(20) << "Edge-block DP" << setw(15) << total_score[1] << setw(12) << best_score[1] << endl;
#endif

    graph g = read_input(cin);
    // auto p1 = solve_random_shuffle(g);
    // auto beg = chrono::high_resolution_clock::now();
    // auto p1 = solve_edge_blocking_dp(g);
    auto p1 = solve_greedy_edgesort(g);
    // auto end = chrono::high_resolution_clock::now();
    // cout << "Time: " << (end - beg).count()/1000000 << '\n';
    print_vector(p1, 0, g.m); cout << '\n';
    // auto p2 = solve_edge_blocking_dp(g);
    // {
    //     vector<int> reference(p1.size());
    //     iota(reference.begin(), reference.end(), 0);
    //     auto p1c = p1;
    //     auto p2c = p2;
    //     sort(p1c.begin(), p1c.end());
    //     sort(p2c.begin(), p2c.end());
    //     // assert(p1c == reference);
    //     cout << p1.size() << '\n' << p2.size() << '\n';
    //     // assert(p2c == reference);
    // }

    // int64_t iterations_passed = 0;
    // while (get_time_seconds() < 4.9) {
    //     shuffle(p.begin(), p.end(), rng);
    //     fp score = calculate_score(p, g);
    //     if (score < min_score) {
    //         p_min = p;
    //         min_score = score;
    //     }
    //     // ++iterations_passed;
    // }
    // cerr << "Iterations passed: " << iterations_passed << '\n';

    // print_vector(p_min, 0, g.m); cout << '\n';

    // cout << "Best score: " << max_score << '\n';
    // cout << "Permutation: "; print_vector(p_max, 0, m); cout << '\n';
}