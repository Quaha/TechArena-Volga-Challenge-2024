#include <iostream>
#include <vector>
#include <numeric>
#include <random>
#include <chrono>
#include <cfloat>
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

void run_tests() {
    int n; cin >> n;
    graph g(n);
    cin >> g.m >> g.M >> g.F;
    for (int i = 0; i < n; ++i) cin >> g.c[i];

    for (int i = 0; i < g.m; ++i) {
        int u,v;
        double s;
        cin >> u >> v >> s;
        --u, --v;
        g.add_edge(u, v, s);
    }

    double c_log_sum = 0.0;
    for (int i = 0; i < n; ++i) {
        c_log_sum += log(g.c[i]);
    }
    c_log_sum *= g.m;

    vector<int> p(g.m), p_min;
    iota(p.begin(), p.end(), 0);
    double min_score = DBL_MAX;
    mt19937_64 rng{random_device{}()};

    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() / 1000000000.0;
    };

    p_min = solve_edge_blocking_dp(g);

    // int64_t iterations_passed = 0;
    // while (get_time_seconds() < 4.9) {
    //     shuffle(p.begin(), p.end(), rng);
    //     double score = check(p, g);
    //     if (score < min_score) {
    //         p_min = p;
    //         min_score = score;
    //     }
    //     // ++iterations_passed;
    // }
    // cerr << "Iterations passed: " << iterations_passed << '\n';

    print_vector(p_min, 0, g.m); cout << '\n';

    // cout << "Best score: " << max_score << '\n';
    // cout << "Permutation: "; print_vector(p_max, 0, m); cout << '\n';
}