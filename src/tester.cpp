#include "includes.h"

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

    const int NUMBER_OF_TESTS     = 1;
    const int NUMBER_OF_SOLUTIONS = 1;

    vector<graph> test_graph(NUMBER_OF_TESTS);
    for (int test_i = 0; test_i < NUMBER_OF_TESTS; ++test_i) {
        string path = "../test_samples/" + to_string(test_i + 1) + ".txt";
        ifstream ifs(path);
        test_graph[test_i] = read_input(ifs);
        ifs.close();
    }

    vector<vector<fp>> scores(NUMBER_OF_TESTS, vector<fp>(NUMBER_OF_SOLUTIONS));
#pragma omp parallel for
    for (int test_i = 0; test_i < NUMBER_OF_TESTS; ++test_i) {
        if ((test_i + 1) % 5 == 0) {
            string out = "Running on test " + to_string(test_i + 1);
            cout << out << endl;
        }
        auto &g = test_graph[test_i];
        scores[test_i] = {
            calculate_score(solve(g), g)
        };
    }

    fp total_score[NUMBER_OF_SOLUTIONS]{};
    int best_score[NUMBER_OF_SOLUTIONS]{};
    for (int test_i = 0; test_i < NUMBER_OF_TESTS; ++test_i) {
        fp c_log_sum = 0.0;
        auto &g = test_graph[test_i];
        for (int i = 0; i < g.n; ++i) {
            c_log_sum += log(g.c[i]);
        }
        c_log_sum *= g.m;
        for (int method_i = 0; method_i < NUMBER_OF_SOLUTIONS; ++method_i) {
            total_score[method_i] += c_log_sum / log(scores[test_i][method_i]);
        }
        ++best_score[min_element(scores[test_i].begin(), scores[test_i].end()) - scores[test_i].begin()];
    }
    cout << setw(20) << "Method" << setw(15) << "Total score" << setw(12) << "Best count" << endl;
    cout << setw(20) << "Random shuffle" << setw(15) << total_score[0] << setw(12) << best_score[0] << endl;
}