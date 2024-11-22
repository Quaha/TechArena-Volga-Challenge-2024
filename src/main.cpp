
#include "solver.h"
#include "tester.h"
#include "param_search.h"
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

