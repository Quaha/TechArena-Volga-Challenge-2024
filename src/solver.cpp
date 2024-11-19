#include <algorithm>
#include <functional>
#include "solver.h"
#include "util.h"
using namespace std;

vector<int> solve_edge_blocking_dp(const graph &g) {
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
    size_t block_size = 16;

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
            size_t num_of_blocks = (cc_edges.size() + block_size - 1) / block_size;
            vector<vector<int>> block_optimal_order(num_of_blocks);
            for (size_t bi = 0; bi < num_of_blocks; ++bi) {
                size_t block_left = block_size * bi;
                size_t block_right = min(block_size * (bi+1), cc_edges.size());
                vector<int> block_ei(cc_edges.begin() + block_left,
                                     cc_edges.begin() + block_right);
                cc_optimal_order = block_dp(g, block_ei);
                block_optimal_order[bi] = cc_optimal_order;
            }
            cc_optimal_order.clear();
            for (const auto &block_order : block_optimal_order) {
                for (int ei : block_order) {
                    cc_optimal_order.push_back(ei);
                }
            }
            // cc_optimal_order = cluster_dp(g, block_optimal_order);
        }
        for (int ei : cc_optimal_order) ans.push_back(ei);
    }

    return ans;
}