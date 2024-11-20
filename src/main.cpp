#include "includes.h"

#include "solver.h"
#include "tester.h"

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

