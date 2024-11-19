#include <algorithm>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
using std::vector;

using fp = long double;

struct edge {
    int u,v;
    fp w;
};

struct graph {
    vector<vector<int>> adj;
    vector<edge> edges;
    vector<fp> c;
    int n,m;
    fp M,F;
    graph();
    graph(int n);
    void add_edge(int u, int v, fp w);
};

struct DSU {
    vector<int> root, sz;
    DSU();
    DSU(int n);
    void init(int n);
    int get(int x);
    void merge(int x, int y);
};
using namespace std;

graph::graph() {}
graph::graph(int n) : n(n), adj(n), c(n) {}
void graph::add_edge(int u, int v, fp w) {
    adj[u].push_back(edges.size());
    adj[v].push_back(edges.size());
    edges.emplace_back(u, v, w);
}

DSU::DSU() {}
DSU::DSU(int n) {
    init(n);
}
void DSU::init(int n) {
    root.resize(n);
    iota(root.begin(), root.end(), 0);
    sz.assign(n, 1);
}
int DSU::get(int x) {
    return x == root[x] ? x : root[x] = get(root[x]);
}
void DSU::merge(int x, int y) {
    x = get(x);
    y = get(y);
    if (x == y) return;
    if (sz[x] > sz[y]) swap(x,y);
    sz[y] += sz[x];
    root[x] = y;
}
using namespace std;

fp penalty(fp c, fp M, fp F) {
    if (c <= M) return 0.0;
    return F * (c - M);
}

fp cost1(fp c, fp c_result, fp M, fp F) {
    return c + c_result + penalty(c_result, M, F);
}

fp cost2(fp c1, fp c2, fp c_result, fp M, fp F) {
    return c1 + c2 + c_result + penalty(c_result, M, F);
}

#ifndef RUN_TESTS
DSU mask_dsu[1 << 16];
vector<fp> mask_c[1 << 16];
fp dp[1 << 16];
int dp_arg[1 << 16];
#endif

vector<int> block_dp(const graph &g, const vector<int> &edges) {
#ifdef RUN_TESTS
    vector<DSU           > mask_dsu(1 << 16);
    vector<vector<fp>> mask_c(1 << 16);
    vector<fp        > dp(1 << 16);
    vector<int           > dp_arg(1 << 16);
#endif
    int m = edges.size();
    mask_dsu[0].init(g.n);
    mask_c[0] = g.c;
    for (int mask = 1; mask < (1 << m); ++mask) {
        dp[mask] = DBL_MAX;
        int bit = __builtin_ctz(mask);
        mask_dsu[mask] = mask_dsu[mask ^ (1 << bit)];
        mask_c[mask] = mask_c[mask ^ (1 << bit)];
        auto [u, v, w] = g.edges[edges[bit]];
        u = mask_dsu[mask].get(u);
        v = mask_dsu[mask].get(v);
        if (u == v) {
            mask_c[mask][u] *= w;
        } else {
            fp c1 = mask_c[mask][u];
            fp c2 = mask_c[mask][v];
            fp c_result = w * c1 * c2;
            mask_dsu[mask].merge(u, v);
            u = mask_dsu[mask].get(u);
            mask_c[mask][u] = c_result;
        }
    }
    for (int mask = 0; mask < (1 << m) - 1; ++mask) {
        for (int bit = 0; bit < m; ++bit) {
            if (mask & (1 << bit)) continue;
            fp cost = 0.0;
            auto [u, v, w] = g.edges[edges[bit]];
            u = mask_dsu[mask].get(u);
            v = mask_dsu[mask].get(v);
            int next_mask = mask ^ (1 << bit);
            if (u == v) {
                fp c = mask_c[mask][u];
                fp c_result = c * w;
                cost = cost1(c, c_result, g.M, g.F);
            } else {
                fp c1 = mask_c[mask][u];
                fp c2 = mask_c[mask][v];
                fp c_result = w * c1 * c2;
                cost = cost2(c1, c2, c_result, g.M, g.F);
            }
            if (cost + dp[mask] < dp[next_mask]) {
                dp_arg[next_mask] = bit;
                dp[next_mask] = cost + dp[mask];
            }
        }
    }
    vector<int> ans;
    for (int mask = (1 << m) - 1; mask > 0; mask ^= (1 << dp_arg[mask])) {
        ans.push_back(edges[dp_arg[mask]]);
    }
    reverse(ans.begin(), ans.end());
    return ans;
}


using namespace std;

struct KahanSum {
    fp s = 0.0;
    fp e = 0.0;
    void add(fp x) {
        fp d = x - e;
        fp t = s + d;
        e = (t - s) - d;
        s = t;
    }
};

fp calculate_score(const vector<int> &p, const graph &g) {
    KahanSum A;
    DSU dsu(g.n);
    vector<fp> c = g.c;

    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = dsu.get(u);
        v = dsu.get(v);
        if (u == v) {
            fp c_result = w * c[u];
            A.add(cost1(c[u], c_result, g.M, g.F));
            c[u] = c_result;
        } else {
            fp c1 = c[u];
            fp c2 = c[v];
            fp c_result = w * c1 * c2;
            A.add(cost2(c[u], c[v], c_result, g.M, g.F));
            dsu.merge(u,v);
            c[dsu.get(u)] = c_result;
        }
    }

    return A.s;
}
using namespace std;

vector<int> solve_edge_blocking_dp(const graph &g) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() / 1000000000.0;
    };

    vector<vector<int>> comp;
    vector<char> was(g.n);
    int comp_num = 0;
    function<void(int)> dfs = [&](int u) {
        was[u] = 1;
        comp.back().push_back(u);
        for (int ei : g.adj[u]) {
            auto &[x, y, w] = g.edges[ei];
            int v = x ^ y ^ u;
            if (was[v]) continue;
            dfs(v);
        }
    };
    for (int i = 0; i < g.n; ++i) {
        if (!was[i]) {
            comp.emplace_back();
            dfs(i);
            ++comp_num;
        }
    }

    vector<int> ans;
    size_t block_size = 15;

    for (const auto &cc : comp) {
        vector<int> cc_edges;
        for (int u : cc) {
            for (int ei : g.adj[u]) {
                cc_edges.push_back(ei);
            }
        }
        sort(cc_edges.begin(), cc_edges.end());
        cc_edges.resize(unique(cc_edges.begin(), cc_edges.end()) - cc_edges.begin());

        vector<int> cc_optimal_order;
        if (cc_edges.size() <= block_size) {
            cc_optimal_order = block_dp(g, cc_edges);
        } else {
            vector<int> order; //order.reserve(cc_edges.size());
            order.resize(cc_edges.size());
            iota(order.begin(), order.end(), 0);
            double min_score = DBL_MAX;
            mt19937_64 rng{random_device{}()};
            while (get_time_seconds() < 4.9) {
                double score = calculate_score(order, g);
                if (score < min_score) {
                    cc_optimal_order = order;
                    min_score = score;
                }
                shuffle(order.begin(), order.end(), rng);
            }
            cc_optimal_order = order;
        }
            // while (get_time_seconds() < 4.2) {
            //     mt19937_64 rng{random_device{}()};
            //     shuffle(cc_edges.begin(), cc_edges.end(), rng);
            //     size_t num_of_blocks = (cc_edges.size() + block_size - 1) / block_size;
            //     vector<vector<int>> block_optimal_order(num_of_blocks);
            //     for (size_t bi = 0; bi < num_of_blocks; ++bi) {
            //         size_t block_left = block_size * bi;
            //         size_t block_right = min(block_size * (bi+1), cc_edges.size());
            //         vector<int> block_ei(cc_edges.begin() + block_left,
            //                             cc_edges.begin() + block_right);
            //         cc_optimal_order = block_dp(g, block_ei);
            //         block_optimal_order[bi] = cc_optimal_order;
            //     }
            //     // cc_optimal_order.clear();
            //     // for (const auto &block_order : block_optimal_order) {
            //     //     for (int ei : block_order) {
            //     //         cc_optimal_order.push_back(ei);
            //     //     }
            //     // }
// 
            //     vector<int> order; order.reserve(cc_edges.size());
            //     double min_score = DBL_MAX;
            //     for (int i = 0; i < 100; ++i) {
            //         order.clear();
            //         shuffle(block_optimal_order.begin(), block_optimal_order.end(), rng);
            //         for (const auto &block : block_optimal_order) for (int ei : block) order.push_back(ei);
            //         double score = calculate_score(order, g);
            //         if (score < min_score) {
            //             cc_optimal_order = order;
            //             min_score = score;
            //         }
            //     }
            // }

            // cc_optimal_order = cluster_dp(g, block_optimal_order);
        for (int ei : cc_optimal_order) ans.push_back(ei);
        // for (int ei : cc_optimal_order) ans.push_back(ei);
    }

    return ans;
}

vector<int> solve_random_shuffle(const graph &g) {
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

vector<int> solve_greedy_edgesort(const graph &g) {
    vector<int> ei(g.m);
    iota(ei.begin(), ei.end(), 0);
    sort(ei.begin(), ei.end(), [&](int i, int j) {return g.edges[i].w < g.edges[j].w;});
    return ei;
}
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
using namespace std;

// #define FILE_INPUT

int main(int argc, char *argv[]) {
#ifdef FILE_INPUT
    // freopen("../test_samples/3.txt", "r", stdin);
    freopen("../input.txt", "r", stdin);
#endif
    run_tests();
}


