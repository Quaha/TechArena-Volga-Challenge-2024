#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <random>
#include <cstdio>
#include <functional>
#include <utility>
#include <bit>
#pragma GCC optimize("Ofast,inline,unroll-loops")

using namespace std;
using fp = long double;
using std::vector;
using std::istream;

#define def_max_pop_size 64
#define def_mutations_per_iter 6
#define def_selection_remain 16
#define def_random_init_size 64
#define def_best_selection_rate 0.6
#define def_crossover_rate 0.7
#define def_repeats 12

#define MINGW

#define PARAM_SEARCH
// #define RUN_TESTS
// #define FILE_INPUT
// #define MSVC
