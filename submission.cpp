#include <algorithm>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <utility>
#include <vector>
using std::vector;

// #define double long double

struct edge {
    int u,v;
    double w;
};

struct graph {
    vector<vector<int>> adj;
    vector<edge> edges;
    vector<double> c;
    int n,m;
    double M,F;
    graph(int n);
    void add_edge(int u, int v, double w);
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

graph::graph(int n) : n(n), adj(n), c(n) {}
void graph::add_edge(int u, int v, double w) {
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

double penalty(double c, double M, double F) {
    if (c <= M) return 0.0;
    return F * (c - M);
}

double cost1(double c, double c_result, double M, double F) {
    return c + c_result + penalty(c_result, M, F);
}

double cost2(double c1, double c2, double c_result, double M, double F) {
    return c1 + c2 + c_result + penalty(c_result, M, F);
}

DSU mask_dsu[1 << 16];
vector<double> mask_c[1 << 16];
double dp[1 << 16];
int dp_arg[1 << 16];

vector<int> block_dp(const graph &g, const vector<int> &edges) {
    int m = edges.size();
    mask_dsu[0].init(g.n);
    mask_c[0] = g.c;
    for (int mask = 1; mask < (1 << m); ++mask) {
        dp[mask] = 1e38;
        int bit = __builtin_ctz(mask);
        mask_dsu[mask] = mask_dsu[mask ^ (1 << bit)];
        mask_c[mask] = mask_c[mask ^ (1 << bit)];
        auto [u, v, w] = g.edges[edges[bit]];
        u = mask_dsu[mask].get(u);
        v = mask_dsu[mask].get(v);
        if (u == v) {
            mask_c[mask][u] *= w;
        } else {
            double c1 = mask_c[mask][u];
            double c2 = mask_c[mask][v];
            double c_result = w * c1 * c2;
            mask_dsu[mask].merge(u, v);
            u = mask_dsu[mask].get(u);
            mask_c[mask][u] = c_result;
        }
    }
    for (int mask = 0; mask < (1 << m) - 1; ++mask) {
        for (int bit = 0; bit < m; ++bit) {
            if (mask & (1 << bit)) continue;
            double cost = 0.0;
            auto [u, v, w] = g.edges[edges[bit]];
            u = mask_dsu[mask].get(u);
            v = mask_dsu[mask].get(v);
            int next_mask = mask ^ (1 << bit);
            if (u == v) {
                double c = mask_c[mask][u];
                double c_result = c * w;
                cost = cost1(c, c_result, g.M, g.F);
            } else {
                double c1 = mask_c[mask][u];
                double c2 = mask_c[mask][v];
                double c_result = w * c1 * c2;
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
using namespace std;

struct KahanSum {
    double s = 0.0;
    double e = 0.0;
    void add(double x) {
        double d = x - e;
        double t = s + d;
        e = (t - s) - d;
        s = t;
    }
};

double check(const vector<int> &p, const graph &g) {
    KahanSum A;
    DSU dsu(g.n);
    vector<double> c = g.c;

    for (int ei : p) {
        auto [u, v, w] = g.edges[ei];
        u = dsu.get(u);
        v = dsu.get(v);
        if (u == v) {
            double c_result = w * c[u];
            A.add(cost1(c[u], c_result, g.M, g.F));
            c[u] = c_result;
        } else {
            double c1 = c[u];
            double c2 = c[v];
            double c_result = w * c1 * c2;
            A.add(cost2(c[u], c[v], c_result, g.M, g.F));
            dsu.merge(u,v);
            c[dsu.get(u)] = c_result;
        }
    }

    return A.s;
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
using namespace std;

// #define FILE_INPUT

int main(int argc, char *argv[]) {
#ifdef FILE_INPUT
    freopen("../input.txt", "r", stdin);
#endif
    run_tests();
}


