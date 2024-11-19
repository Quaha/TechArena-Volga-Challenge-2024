#include <algorithm>
#include <functional>
#include <chrono>
#include <numeric>
#include <random>
#include <chrono>
#include <cfloat>
#include "solver.h"
#include "checker.h"
#include "util.h"
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