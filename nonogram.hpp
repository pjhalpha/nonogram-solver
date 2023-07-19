#ifndef PJHALPHA_NONOGRAM_H
#define PJHALPHA_NONOGRAM_H

#include "ngenv.hpp"
#include "ngtable.hpp"
#include "ngline.hpp"

class Nonogram {
private:
    friend class NonogramTable;
    friend class NonogramLine;

    Nonogram(void); // Constructor for creating singleton instance.
    Nonogram(const Nonogram* const, const uint16_t, const uint16_t); // Constructor for creating a new branch from the previous table.
    ~Nonogram(void) = default;
    Nonogram(const Nonogram&) = delete;
    Nonogram &operator=(const Nonogram&) = delete;
    
    static Nonogram base; // Singleton instance.
    static uint16_t size[2]; // Size of the table ([vector(0 row, 1 column)]).
    static uint16_t offset[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]; // Prefix sum of the clue ([vector][vector index][clue index]).
    static uint16_t clue_size[2][MAX_NG_SIZE]; // Size of the clue ([vector][vector index]).
    static uint16_t margin[2][MAX_NG_SIZE]; // Margin based on size of the table and the clue ([vector][vector index]).
    static bool validity; // Validity of the clue.
    static bool solubility; // Solubility of the nonogram.
    static bool input(std::istream&); // Input the clue from istream and validate, return validity.

    NonogramTable table;
    NonogramLine line;
    bool solve(void); // Solve nonogram until the table is no longer updated, return solubility.
    bool infer(void); // Fix one line arbitrarily when the table is no longer updated, return solubility.

public:
    static bool solve(std::istream&); // Input the clue from istream, return solubility.
    static bool isValid(void) noexcept; // Return validity.
    static bool isSolved(void) noexcept; // Return solubility.
    static void print(std::ostream&); // Print the table.
};

#endif // PJHALPHA_NONOGRAM_H
