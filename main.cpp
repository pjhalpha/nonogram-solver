#include <iostream>
#include "nonogram.hpp"

using namespace std;

int main(void) {
    cin.tie(0)->sync_with_stdio(0);

    // Input a clue and print the solution if any solutions are found.
    if (Nonogram::solve(cin)) Nonogram::print(cout);

    return 0;
}
