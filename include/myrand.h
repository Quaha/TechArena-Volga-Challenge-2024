#pragma once

#include "includes.h"

class RandomGenerator {
private:
    uint64_t seed;
    uint64_t a;
    uint64_t c;
    const static uint64_t m = (1ULL << 63);
    uint64_t state;

public:

    using result_type = uint64_t;

    RandomGenerator(uint64_t seed,
        uint64_t a = 6364136223846793005ULL,
        uint64_t c = 10139042347423ULL)
        : seed(seed), state(seed), a(a), c(c) {}

    uint64_t operator()() {
        state = (a * state + c) % m;
        return state;
    }

    constexpr static uint64_t min() { return 0; }
    constexpr static uint64_t max() { return m - 1ULL; }
};