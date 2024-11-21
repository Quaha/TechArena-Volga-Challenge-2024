
#include "solver.h"
#include "tester.h"
#include "param_search.h"
using namespace std;

int main(int argc, char *argv[]) {
#ifndef PARAM_SEARCH
#ifdef INPUT
    //freopen("../input.txt", "r", stdin);
    solve();
#endif
#ifndef INPUT
    run_tests();
#endif
#endif

#ifdef PARAM_SEARCH
    param_search();
#endif
    return 0;
}

