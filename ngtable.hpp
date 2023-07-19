#ifndef PJHALPHA_NGTABLE_H
#define PJHALPHA_NGTABLE_H

#include "ngenv.hpp"

class NonogramTable {
private:
    friend class Nonogram;
    friend class NonogramLine;

    NonogramTable(void) = delete;
    NonogramTable(NonogramLine* const); // Constructor for setting constants.
    ~NonogramTable(void) = default;
    NonogramTable(const NonogramTable&) = delete;
    NonogramLine &operator=(const NonogramTable&) = delete; 
    
    static const uint16_t (&size)[2]; // Reference of Nonogram::size.
    static const uint16_t (&offset)[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]; // Reference of Nonogram::offset.
    static const uint16_t (&clue_size)[2][MAX_NG_SIZE]; // Reference of Nonogram::clue size.
    static const uint16_t (&margin)[2][MAX_NG_SIZE]; // Reference of Nonogram::margin.

    NonogramLine* const line;
    uint16_t base[MAX_NG_SIZE][MAX_NG_SIZE]; // Table ([row][column], 0b10 black, 0b01 white, 0b00 gray).
    uint32_t remain; // Number of grays in the table.
    uint32_t state; // Number of lines having possibility of being updated.
    uint16_t line_remain[2][MAX_NG_SIZE]; // Number of grays in line ([vector][vector index]).
    bool line_state[2][MAX_NG_SIZE]; // Possibility of being updated ([vector][vector index]).
    uint16_t get(const bool, const uint16_t, const uint16_t) const noexcept; // Get a pixel of tthe able.
    void set(const bool, const uint16_t, const uint16_t, const uint16_t); // Set a pixel of the table.
    void fill(const bool, const uint16_t, const uint16_t, const uint16_t, const uint16_t); // Fill the table.
    void init(void); // Initialize and quickly fill in some pixles of the table.
    void copy(const NonogramTable&); // Copy the table.
    void merge(const uint16_t, const uint16_t); // Merge the table and a solved line.
    bool verify(void); // Verify the found solution, return solubility.
};

#endif // PJHALPHA_NGTABLE_H
