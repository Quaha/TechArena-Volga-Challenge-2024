#define MINGW
#include <algorithm>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>

using namespace std;
using fp = long double;
using std::vector;
using std::istream;

#define def_max_pop_size 256
#define def_mutations_per_iter 24
#define def_selection_remain 32
#define def_random_init_size 2048
#define def_best_selection_rate 0.6
#define def_crossover_rate 0.7
#define def_repeats 12

// #define PARAM_SEARCH
// #define RUN_TESTS
// #define FILE_INPUT



struct edge {
    int u,v;
    fp w;
    edge(int u, int v, fp w) : u(u), v(v), w(w) {};
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

graph read_input(istream &ist);

struct DSU {
    vector<int> root, sz;
    DSU();
    DSU(int n);
    void init(int n);
    int get(int x);
    void merge(int x, int y);
};

vector<vector<int>> get_connected_components(const graph &g);


graph::graph() {}
graph::graph(int n) : n(n), adj(n), c(n) {}
void graph::add_edge(int u, int v, fp w) {
    adj[u].push_back(edges.size());
    adj[v].push_back(edges.size());
    edges.emplace_back(u, v, w);
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

vector<vector<int>> get_connected_components(const graph &g) {
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
    return comp;
}


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

struct KahanSum {
    fp s = 0.0;
    fp e = 0.0;
    void add(fp x) {
        s += x;
        fp d = x - e;
        fp t = s + d;
        e = (t - s) - d;
        s = t;
    }
};

// DSU score_dsu(200);
// fp score_c[200];

fp calculate_score(const vector<int> &p, const graph &g) {
    KahanSum A;
    DSU score_dsu(g.n);
    score_dsu.init(g.n);
    vector<fp> score_c = g.c;
    // for (int i = 0; i < g.n; ++i) score_c[i] = g.c[i];

    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = score_dsu.get(u);
        v = score_dsu.get(v);
        if (u == v) {
            fp c_result = w * score_c[u];
            A.add(cost1(score_c[u], c_result, g.M, g.F));
            score_c[u] = c_result;
        } else {
            fp c1 = score_c[u];
            fp c2 = score_c[v];
            fp c_result = w * c1 * c2;
            A.add(cost2(score_c[u], score_c[v], c_result, g.M, g.F));
            score_dsu.merge(u,v);
            score_c[score_dsu.get(u)] = c_result;
        }
    }

    return A.s;
}

using namespace std;
const int MAX_M = 21;

#ifndef RUN_TESTS
DSU mask_dsu[1 << MAX_M];
vector<fp> mask_c[1 << MAX_M];
fp dp[1 << MAX_M];
int dp_arg[1 << MAX_M];
#endif

vector<int> block_dp(const graph &g, const vector<int> &edges) {
#ifdef RUN_TESTS
    vector<DSU           > mask_dsu(1 << MAX_M);
    vector<vector<fp>> mask_c(1 << MAX_M);
    vector<fp        > dp(1 << MAX_M);
    vector<int           > dp_arg(1 << MAX_M);
#endif
    int m = edges.size();
#ifdef MINGW
    mask_dsu[0].init(g.n);
    mask_c[0] = g.c;
    for (int mask = 1; mask < (1 << m); ++mask) {
        dp[mask] = numeric_limits<fp>::max();
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
#endif
#ifndef MINGW
    vector<int> ans;
    for (int i = 1; i <= m; i++) {
        ans.push_back(i);
    }
#endif
    return ans;
}



vector<int> solve_random_shuffle(const graph &g) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() * 1e-9;
    };
    vector<int> p(g.m), p_min;
    iota(p.begin(), p.end(), 0);
    p_min = p;
    fp min_score = numeric_limits<fp>::max();
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
                          int best_selection_rate,
                          int crossover_rate,
                          int repeats) {
    auto start = chrono::high_resolution_clock::now();
    auto get_time_seconds = [&]() {
        auto current = chrono::high_resolution_clock::now();
        return (current - start).count() * 1e-9;
    };
 
    double time_limit = 4.9 / repeats;

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
    vector<int> map(g.m), map_yx(g.m);
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
    p = solve_genetic(g,
                      def_max_pop_size,
                      def_mutations_per_iter,
                      def_selection_remain,
                      def_random_init_size,
                      def_best_selection_rate,
                      def_crossover_rate,
                      def_repeats);
#endif
#ifdef MINGW
    if (g.m <= 21) {
        p = solve_dp(g);
    }
    else {
        // p = solve_random_shuffle(g);
        p = solve_genetic(g,
                          def_max_pop_size,
                          def_mutations_per_iter,
                          def_selection_remain,
                          def_random_init_size,
                          def_best_selection_rate,
                          def_crossover_rate,
                          def_repeats);
    }
#endif

    for (int i = 0; i < p.size(); ++i) {
        cout << p[i] + 1 << " ";
    }
    cout << '\n';
}


using namespace std;

int main(int argc, char *argv[]) {
#if defined(PARAM_SEARCH)
    param_search();
#elif defined(RUN_TESTS)
    run_tests();
#else
#if defined(FILE_INPUT)
    cout << "Run test case from file" << endl;
    if (freopen("../test_samples/1.txt", "r", stdin) == NULL) {
        cout << "Can't open input file" << endl;
        return -1;
    }
    cout << "Opened file, running test..." << endl;
#endif
    solve();
#endif
    return 0;
}


