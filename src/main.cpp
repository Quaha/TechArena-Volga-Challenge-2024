#include "includes.h"

#include "solver.h"
#include "tester.h"

using namespace std;

int main(int argc, char *argv[]) {
#ifdef INPUT
    //freopen("../input.txt", "r", stdin);
    solve();
#endif
#ifndef INPUT
    run_tests();
#endif

    return 0;
}

