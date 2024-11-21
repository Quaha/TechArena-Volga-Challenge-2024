#include <iostream>
#include <fstream>
#include <vector>
#include <numeric>
#include <cmath>
#include <cassert>
#include <chrono>
#include <cfloat>
#include <iomanip>
#include <algorithm>
#include <random>
#include <cstdio>
#include <functional>
#include <utility>

using namespace std;
using fp = long double;
using std::vector;
using std::istream;

#define opt_max_pop_size 768
#define opt_mutations_per_iter 96
#define opt_selection_remain 192
#define opt_random_init_size 5120 

//#define MINGW
// #define INPUT
