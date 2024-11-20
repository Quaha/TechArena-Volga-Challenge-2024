#include "checker.h"
#include "tester.h"
#include <cstdio>
using namespace std;

// #define FILE_INPUT

int main(int argc, char *argv[]) {
#ifdef FILE_INPUT
    freopen("../test_samples/3.txt", "r", stdin);
    // freopen("../input.txt", "r", stdin);
#endif
    run_tests();
}

