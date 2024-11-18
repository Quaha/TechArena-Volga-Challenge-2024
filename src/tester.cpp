#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <chrono>
#include "tester.h"
#include "checker.h"
using namespace std;

template <typename T>
void print_vector(const vector<T> &v, int b, int e) {
    for (int i = b; i < e; ++i) {
        cout << v[i]+1 << ' ';
    }
}

void run_tests() {
    int n,m; cin >> n >> m;
    double M, F; cin >> M >> F;

    vector<double> c(n);
    for (int i = 0; i < n; ++i) cin >> c[i];

    graph g(n);
    for (int i = 0; i < m; ++i) {
        int u,v;
        double s;
        cin >> u >> v >> s;
        --u, --v;
        g.add_edge(u, v, s);
    }

    vector<int> p(m), p_max;
    iota(p.begin(), p.end(), 0);
    double max_score = 0.0;
    int max_iter = 100000;
    mt19937_64 rng{random_device{}()};

    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() / 1000000000.0;
    };

    while (get_time_seconds() < 4.9) {
        shuffle(p.begin(), p.end(), rng);
        double score = check(p, g, c, n, m, M, F);
        if (score > max_score) {
            p_max = p;
            max_score = score;
        }
    }

    print_vector(p_max, 0, m); cout << '\n';

    // cout << "Best score: " << max_score << '\n';
    // cout << "Permutation: "; print_vector(p_max, 0, m); cout << '\n';
}