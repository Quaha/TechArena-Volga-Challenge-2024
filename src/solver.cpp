#include "includes.h"

#include "solver.h"
#include "checker.h"
#include "tester.h"
#include "block_dp.h"

vector<int> solve_edge_blocking_dp(const graph &g) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() / 1000000000.0;
    };

    auto comp = get_connected_components(g);

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
            fp min_score = DBL_MAX;
            mt19937_64 rng{random_device{}()};
            while (get_time_seconds() < 4.9) {
                fp score = calculate_score(order, g);
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
            //     fp min_score = DBL_MAX;
            //     for (int i = 0; i < 100; ++i) {
            //         order.clear();
            //         shuffle(block_optimal_order.begin(), block_optimal_order.end(), rng);
            //         for (const auto &block : block_optimal_order) for (int ei : block) order.push_back(ei);
            //         fp score = calculate_score(order, g);
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

    while (get_time_seconds() < 4.93) {
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

vector<int> solve_dp(const graph &g) {
    vector<int> edges(g.m);
    iota(edges.begin(), edges.end(), 0);
    return block_dp(g, edges);
}

// genetic algorithm

vector<int> solve_genetic(const graph &g) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() / 1000000000.0;
    };
    
    const int iterations = 1000;
    const int max_pop_size = 500;
    const int mutations_per_iter = 20;
    const int selection_remain = 100;

    vector<pair<fp, vector<int>>> pop;
    pop.reserve(max_pop_size);

    vector<int> p_ans(g.m);
    fp min_score = DBL_MAX;
    auto selection = [&]() {
        sort(pop.begin(), pop.end(), [&](const auto &a, const auto &b) {
            return a.first < b.first;
        });
        pop.resize(selection_remain);
        if (pop[0].first < min_score) {
            min_score = pop[0].first;
            p_ans = pop[0].second;
        }
    };

    mt19937_64 rng{random_device{}()};
    vector<int> id(g.m); iota(id.begin(), id.end(), 0);
    uniform_int_distribution<int> unif_pop(0, selection_remain - 1);
    uniform_int_distribution<int> unif_pos(0, g.m - 1);
    vector<int> map(g.m), map_yx(g.m);
    auto crossover = [&](const auto &x, const auto &y, int l, int r) {
        vector<int> p(g.m);

        for (int i = 0; i < g.m; ++i) map_yx[i] = -1;
        for (int i = l; i <= r; ++i) {
            p[i] = y[i];
            map_yx[y[i]] = x[i];
        }
        for (int i = 0; i < l; ++i) {
            p[i] = x[i];
            while (map_yx[p[i]] != -1) {
                p[i] = map_yx[p[i]];
            }
        }
        for (int i = r + 1; i < g.m; ++i) {
            p[i] = x[i];
            while (map_yx[p[i]] != -1) {
                p[i] = map_yx[p[i]];
            }
        }

        return p;
    };

    vector<int> p(g.m);
    iota(p.begin(), p.end(), 0);
    for (int i = 0; i < max_pop_size; ++i) {
        shuffle(p.begin(), p.end(), rng);
        pop.emplace_back(calculate_score(p, g), p);
    }
    selection();
    // cout << "Iteration 0, best score: " << pop[0].first << endl;

    // for (int ga_iter = 0; ga_iter < iterations; ++ga_iter) {
    while (get_time_seconds() < 4.85) {
        for (int i = selection_remain; i < max_pop_size; ++i) {
            int l = unif_pos(rng);
            int r = unif_pos(rng);
            if (l > r) swap(l, r);
            auto p = crossover(pop[unif_pop(rng)].second, pop[unif_pop(rng)].second, l, r);
            pop.emplace_back(calculate_score(p, g), p);
        }
        for (int mut_i = 0; mut_i < mutations_per_iter; ++mut_i) {
            int pop_i = unif_pop(rng);
            int pos1 = unif_pos(rng), pos2 = unif_pos(rng);
            swap(pop[pop_i].second[pos1], pop[pop_i].second[pos2]);
            pop[pop_i].first = calculate_score(pop[pop_i].second, g);
        }
        selection();
        // cout << "Iteration " << ga_iter + 1 << ", best score: " << pop[0].first << endl;
    }

    return p_ans;
}

void solve() {
    int n; cin >> n;
    graph g(n);
    cin >> g.m >> g.M >> g.F;
    for (int i = 0; i < n; ++i) cin >> g.c[i];

    for (int i = 0; i < g.m; ++i) {
        int u, v;
        fp s;
        cin >> u >> v >> s;
        g.add_edge(u - 1, v - 1, s);
    }

    vector<int> p;

#ifndef MINGW:
    p = solve_genetic(g);
#endif
#ifdef MINGW
    if (g.m <= 21) {
        p = solve_dp(g);
    }
    else {
        // p = solve_random_shuffle(g);
        p = solve_genetic(g);
    }
#endif

    for (int i = 0; i < p.size(); ++i) {
        cout << p[i] + 1 << " ";
    }
    cout << '\n';
}
