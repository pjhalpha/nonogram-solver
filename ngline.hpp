#ifndef PJHALPHA_NGLINE_H
#define PJHALPHA_NGLINE_H

#include "ngenv.hpp"

class NonogramLine {
private:
    friend class Nonogram;
    friend class NonogramTable;

    NonogramLine(void) = delete;
    NonogramLine(NonogramTable* const); // Constructor for set constant.
    ~NonogramLine(void) = default;
    NonogramLine(const NonogramLine&) = delete;
    NonogramLine &operator=(const NonogramLine&) = delete;
    
    static const uint16_t (&size)[2]; // Reference of nonogram size.
    static const uint16_t (&offset)[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]; // Reference of nonogram offset.
    static const uint16_t (&clue_size)[2][MAX_NG_SIZE]; // Reference of nonogram clue size.
    static const uint16_t (&margin)[2][MAX_NG_SIZE]; // Reference of nonogram margin.

    NonogramTable* const table;
    std::bitset<MAX_NG_SIZE << 1> base; // Line for being merged with the table.
    std::bitset<MAX_NG_SIZE << 1> temp; // Line for storing solution cases.
    uint16_t black[MAX_NG_SIZE + 1]; // Prefix sum of filled pixels ([clue index]).
    uint16_t white[MAX_NG_SIZE + 1]; // Prefix sum of blank pixels ([clue index]).
    std::bitset<MAX_NG_SIZE> possible[MAX_NG_CLUE_SIZE + 2]; // Solubility ([clue index][shift index]).
    std::bitset<MAX_NG_SIZE> impossible[MAX_NG_CLUE_SIZE + 1]; // Unsolubility ([clue index][shift index]).
    uint16_t shift[MAX_NG_CLUE_SIZE + 2]; // Clue offset ([clue index]).
    std::bitset<MAX_NG_CLUE_SIZE + 2> find; // Solubility according to shift index ([clue index]).
    void init(const uint16_t, const uint16_t); // Initialize a line.
    bool solve(const uint16_t, const uint16_t); // Solve a line, return solubility.
    bool infer(const Nonogram* const, const uint16_t, const uint16_t); // Confirm a line arbitrarily, return solubility.
};

#endif
