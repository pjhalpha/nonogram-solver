#ifndef PJHALPHA_NGTABLE_H
#define PJHALPHA_NGTABLE_H

#include "ngenv.hpp"

class NonogramTable {
private:
    friend class Nonogram;
    friend class NonogramLine;

    NonogramTable(void) = delete;
    NonogramTable(NonogramLine* const); // Constructor for set constant.
    ~NonogramTable(void) = default;
    NonogramTable(const NonogramTable&) = delete;
    NonogramLine &operator=(const NonogramTable&) = delete; 
    
    static const uint16_t (&size)[2]; // Reference of nonogram size.
    static const uint16_t (&offset)[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]; // Reference of nonogram offset.
    static const uint16_t (&clue_size)[2][MAX_NG_SIZE]; // Reference of nonogram clue size.
    static const uint16_t (&margin)[2][MAX_NG_SIZE]; // Reference of nonogram margin.

    NonogramLine* const line;
    std::bitset<MAX_NG_SIZE << 1> base[MAX_NG_SIZE]; // Table (0b10 black, 0b01 white, 0b00 gray).
    uint32_t remain; // Number of grays in table.
    uint32_t state; // Number of lines having possibility of being updated.
    uint16_t line_remain[2][MAX_NG_SIZE]; // Number of grays in line ([vector][vector index]).
    std::bitset<MAX_NG_SIZE> line_state[2]; // Possibility of being updated ([vector][vector index]).
    uint16_t get(const bool, const uint16_t, const uint16_t) const noexcept; // Get a pixel binary bit of table.
    void set(const bool, const uint16_t, const uint16_t, const uint16_t); // Set a pixel binary bit of table.
    void fill(const bool, const uint16_t, const uint16_t, const uint16_t, const uint16_t); // Fill table.
    void init(void); // Initialize and fill in some pixles quickly table.
    void merge(const uint16_t, const uint16_t); // Merge table and a solved line.
    bool verify(void); // Verify the found solution, return solubility.
    void copy(const NonogramTable&); // Copy table.
};

#endif
