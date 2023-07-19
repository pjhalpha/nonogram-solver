#ifndef PJHALPHA_NGLINE_H
#define PJHALPHA_NGLINE_H

#include "ngenv.hpp"

class NonogramLine {
private:
    friend class Nonogram;
    friend class NonogramTable;

    NonogramLine(void) = delete;
    NonogramLine(NonogramTable* const); // Constructor for setting constants.
    ~NonogramLine(void) = default;
    NonogramLine(const NonogramLine&) = delete;
    NonogramLine &operator=(const NonogramLine&) = delete;
    
    static const uint16_t (&size)[2]; // Reference of Nonogram::size.
    static const uint16_t (&offset)[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]; // Reference of Nonogram::offset.
    static const uint16_t (&clue_size)[2][MAX_NG_SIZE]; // Reference of Nonogram::clue size.
    static const uint16_t (&margin)[2][MAX_NG_SIZE]; // Reference of Nonogram::margin.

    NonogramTable* const table;
    uint16_t base[MAX_NG_SIZE + 1]; // Line (0b10 black, 0b01 white, 0b00 gray).
    uint16_t color[2][MAX_NG_SIZE + 2]; // Prefix sum of pixels ([0 white, 1 black][index]).
    uint16_t shift[2][MAX_NG_CLUE_SIZE + 2]; // Shift index ([0 current, 1 previous][clue index]).
    bool cache[2][MAX_NG_CLUE_SIZE + 2][MAX_NG_SIZE]; // Solubility according to shift index ([0 impossible, 1 possible][clue index][shift index]).
    bool subcache[MAX_NG_CLUE_SIZE + 2]; // Solubility according to clue index ([clue index]).
    void init(const uint16_t, const uint16_t); // Initialize variables.
    void fill(const uint16_t, const uint16_t, const uint16_t); // Fill a line using bitwise AND.
    bool solve(const uint16_t, const uint16_t); // Solve a line, return solubility.
    bool infer(const Nonogram* const, const uint16_t, const uint16_t); // Fix a line arbitrarily, return solubility.
};

#endif // PJHALPHA_NGLINE_H
