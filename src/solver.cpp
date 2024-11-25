#include "includes.h"
#include "solver.h"
#include "checker.h"
#include "tester.h"
#include "block_dp.h"

vector<int> solve_dp(const graph &g) {
    vector<int> edges(g.m);
    iota(edges.begin(), edges.end(), 0);
    return block_dp(g, edges);
}

// genetic algorithm

vector<int> solve_genetic(const graph &g,
                          int max_pop_size,
                          int mutations_per_iter,
                          int selection_remain,
                          int random_init_size,
                          double best_selection_rate,
                          double crossover_rate,
                          int repeats) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() * 1e-9;
    };
 
    double time_limit = 4.85 / repeats;

    vector<pair<fp, vector<int>>> pop;
    pop.reserve(max(random_init_size, max_pop_size));
 
    vector<int> p_ans(g.m);
    mt19937_64 rng{random_device{}()};
    fp min_score = numeric_limits<fp>::max();
    auto selection = [&]() {
        sort(pop.begin(), pop.end(), [&](const auto &a, const auto &b) {
            return a.first < b.first;
        });
        const int best_border = best_selection_rate * selection_remain;
        shuffle(pop.begin() + best_border, pop.end(), rng);
        pop.resize(selection_remain);
        if (pop[0].first < min_score) {
            min_score = pop[0].first;
            p_ans = pop[0].second;
        }
    };
 
    vector<int> p(g.m);
    vector<int> id(g.m); iota(id.begin(), id.end(), 0);
    uniform_int_distribution<int> unif_pop(0, selection_remain - 1);
    uniform_int_distribution<int> unif_pos(0, g.m - 1);
    int map_yx[200]{};
    auto crossover = [&](const auto &x, const auto &y, int l, int r) {
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
    };
 
 
    mt19937_64 pos_rng{random_device{}()};
    using param_type = uniform_int_distribution<int>::param_type;
    iota(p.begin(), p.end(), 0);
    
    
    for (int ga_repeat = 0; ga_repeat < repeats; ++ga_repeat) {
        start = chrono::high_resolution_clock::now();
        pop.clear();
        
        for (int i = 0; i < random_init_size; ++i) {
            shuffle(p.begin(), p.end(), rng);
            pop.emplace_back(calculate_score(p, g), p);
        }
        selection();
        
        while (get_time_seconds() < time_limit) {
            const int crossover_cnt = crossover_rate * (max_pop_size - selection_remain);
            const int mutation_cnt = max_pop_size - crossover_cnt - selection_remain;
            for (int i = 0; i < crossover_cnt; ++i) {
                unif_pop.param(param_type(0, pop.size() - 1));
                int l = unif_pos(pos_rng);
                int r = unif_pos(pos_rng);
                if (l > r) swap(l, r);
                crossover(pop[unif_pop(rng)].second, pop[unif_pop(rng)].second, l, r);
                pop.emplace_back(calculate_score(p, g), p);
            }
            for (int i = 0; i < mutation_cnt; ++i) {
                unif_pop.param(param_type(0, pop.size() - 1));
                vector<int> p = pop[unif_pop(rng)].second;
                int pos1 = unif_pos(rng), pos2 = unif_pos(rng);
                swap(p[pos1], p[pos2]);
                pop.emplace_back(calculate_score(p, g), p);
            }
            for (int mut_i = 0; mut_i < mutations_per_iter; ++mut_i) {
                int pop_i = unif_pop(rng);
                int pos1 = unif_pos(pos_rng), pos2 = unif_pos(pos_rng);
                swap(pop[pop_i].second[pos1], pop[pop_i].second[pos2]);
                pop[pop_i].first = calculate_score(pop[pop_i].second, g);
            }
            selection();
        }
    }
    return p_ans;
}

vector<int> solve_genetic(const graph &g) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() * 1e-9;
    };
 
    constexpr double time_limit = 4.9 / def_repeats;

    vector<pair<fp, vector<int>>> pop;
    pop.reserve(max(def_random_init_size, def_max_pop_size));
 
    vector<int> p_ans(g.m);
    mt19937_64 rng{random_device{}()};
    fp min_score = numeric_limits<fp>::max();
    auto selection = [&]() {
        sort(pop.begin(), pop.end(), [&](const auto &a, const auto &b) {
            return a.first < b.first;
        });
        const int best_border = def_best_selection_rate * def_selection_remain;
        shuffle(pop.begin() + best_border, pop.end(), rng);
        pop.resize(def_selection_remain);
        if (pop[0].first < min_score) {
            min_score = pop[0].first;
            p_ans = pop[0].second;
        }
    };
 
    vector<int> p(g.m);
    vector<int> id(g.m); iota(id.begin(), id.end(), 0);
    uniform_int_distribution<int> unif_pop(0, def_selection_remain - 1);
    uniform_int_distribution<int> unif_pos(0, g.m - 1);
    int map_yx[200]{};
    auto crossover = [&](const auto &x, const auto &y, int l, int r) {
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
    };
 
 
    mt19937_64 pos_rng{random_device{}()};
    using param_type = uniform_int_distribution<int>::param_type;
    iota(p.begin(), p.end(), 0);
    
    
    for (int ga_repeat = 0; ga_repeat < def_repeats; ++ga_repeat) {
        start = chrono::high_resolution_clock::now();
        pop.clear();
        
        for (int i = 0; i < def_random_init_size; ++i) {
            shuffle(p.begin(), p.end(), rng);
            pop.emplace_back(calculate_score(p, g), p);
        }
        selection();
        
        constexpr int crossover_cnt = def_crossover_rate * (def_max_pop_size - def_selection_remain);
        constexpr int mutation_cnt = def_max_pop_size - crossover_cnt - def_selection_remain;
        while (get_time_seconds() < time_limit) {
            for (int i = 0; i < crossover_cnt; ++i) {
                unif_pop.param(param_type(0, pop.size() - 1));
                int l = unif_pos(pos_rng);
                int r = unif_pos(pos_rng);
                if (l > r) swap(l, r);
                crossover(pop[unif_pop(rng)].second, pop[unif_pop(rng)].second, l, r);
                pop.emplace_back(calculate_score(p, g), p);
            }
            for (int i = 0; i < mutation_cnt; ++i) {
                unif_pop.param(param_type(0, pop.size() - 1));
                vector<int> p = pop[unif_pop(rng)].second;
                int pos1 = unif_pos(rng), pos2 = unif_pos(rng);
                swap(p[pos1], p[pos2]);
                pop.emplace_back(calculate_score(p, g), p);
            }
            for (int mut_i = 0; mut_i < def_mutations_per_iter; ++mut_i) {
                int pop_i = unif_pop(rng);
                int pos1 = unif_pos(pos_rng), pos2 = unif_pos(pos_rng);
                swap(pop[pop_i].second[pos1], pop[pop_i].second[pos2]);
                pop[pop_i].first = calculate_score(pop[pop_i].second, g);
            }
            selection();
        }
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

#ifndef MINGW
    p = solve_genetic(g);
#endif
#ifdef MINGW
    if (g.m <= 21) {
        p = solve_dp(g);
    }
    else {
        p = solve_genetic(g);
    }
#endif

    for (int i = 0; i < p.size(); ++i) {
        cout << p[i] + 1 << " ";
    }
    cout << '\n';
}
