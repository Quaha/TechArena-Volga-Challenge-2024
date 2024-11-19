#include "includes.h"

#include "solver.h"
#include "tester.h"

using namespace std;

#define INPUT

int main(int argc, char *argv[]) {
#ifdef INPUT
    //freopen("../input.txt", "r", stdin);
    //freopen("../output.txt", "w", stdout);
    solve();
#endif
#ifndef INPUT
    run_tests();
#endif
}

