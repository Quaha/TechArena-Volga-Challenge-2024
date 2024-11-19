#include "includes.h"

#include "solver.h"
#include "checker.h"

vector<int> solve(const graph &g) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() / 1000000000.0;
    };
    vector<int> p(g.m), p_min;
    iota(p.begin(), p.end(), 0);
    p_min = p;

    fp min_score = DBL_MAX;

    mt19937_64 rng{random_device{}()};

    while (get_time_seconds() < 4.9) {
        shuffle(p.begin(), p.end(), rng);
        fp score = calculate_score(p, g);
        if (score < min_score) {
            p_min = p;
            min_score = score;
        }
    }

    return p_min;
}

void solve() {

    int n;
    cin >> n;

    graph g(n);

    cin >> g.m >> g.M >> g.F;
    for (int i = 0; i < n; ++i) {
        cin >> g.c[i];
    }

    for (int i = 0; i < g.m; ++i) {
        int u, v;
        fp s;
        cin >> u >> v >> s;
        g.add_edge(u - 1, v - 1, s);
    }

    vector<int> result = solve(g);

    for (int i = 0; i < result.size(); ++i) {
        cout << result[i] + 1 << " ";
    }
}