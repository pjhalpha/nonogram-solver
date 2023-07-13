#include "nonogram.hpp"

using namespace std;

int main(void) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Input a clue and print the solution if any solutions are found.
    Nonogram::solve(cin);
    if (Nonogram::isSolved()) Nonogram::print(cout);

    return 0;
}
