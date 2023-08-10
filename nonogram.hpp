#ifndef PJHALPHA_NONOGRAM_H
#define PJHALPHA_NONOGRAM_H

#include <iostream>
#include <sstream>

#define MAX_NG_SIZE 255 // Maximum table size, expressed using 2 bytes (MAX_NG_SIZE by MAX_NG_SIZE matrix).
#define MAX_NG_CLUE_SIZE (MAX_NG_SIZE + 1 >> 1) // Maximum clue size.
static_assert(MAX_NG_SIZE < 0x10000);

char printBinBit(const uint16_t); // Return character for a pixel

class Nonogram {
private:
    Nonogram(void) = default;
    Nonogram(const Nonogram* const, const uint16_t, const uint16_t); // Create a new branch from the previous table.
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

    void merge(const uint16_t, const uint16_t); // Merge the table and a solved line.
    bool solve(void); // Solve nonogram until the table is no longer updated, return solubility.
    bool infer(void); // Fix one line arbitrarily when the table is no longer updated, return solubility.
    bool verify(void); // Verify the found solution, return solubility.

    uint16_t table[MAX_NG_SIZE][MAX_NG_SIZE]; // Table ([row][column], 0b10 black, 0b01 white, 0b00 gray).
    uint32_t remain; // Number of grays in the table.
    uint32_t state; // Number of lines having possibility to be updated.
    uint16_t line_remain[2][MAX_NG_SIZE]; // Number of grays in line ([vector][vector index]).
    bool line_state[2][MAX_NG_SIZE]; // Possibility to be updated ([vector][vector index]).
    uint16_t get(const bool, const uint16_t, const uint16_t) const; // Get a pixel of the table.
    void set(const bool, const uint16_t, const uint16_t, const uint16_t); // Set a pixel of the table.
    void fill(const bool, const uint16_t, const uint16_t, const uint16_t, const uint16_t); // Fill the table.
    void init(void); // Initialize and quickly fill in some pixles of the table.
    void copy(const Nonogram* const); // Copy the table.

    uint16_t line[MAX_NG_SIZE + 1]; // Line ([index], 0b10 black, 0b01 white, 0b00 gray).
    uint16_t color[2][MAX_NG_SIZE + 2]; // Prefix sum of pixels ([0 white, 1 black][index]).
    uint16_t shift[2][MAX_NG_CLUE_SIZE + 2]; // Shift index ([0 current, 1 previous][clue index]).
    bool cache[2][MAX_NG_CLUE_SIZE + 2][MAX_NG_SIZE]; // Solubility according to shift index ([0 impossible, 1 possible][clue index][shift index]).
    bool subcache[MAX_NG_CLUE_SIZE + 2]; // Solubility according to clue index ([clue index]).
    void initLine(const uint16_t, const uint16_t); // Initialize variables.
    void fillLine(const uint16_t, const uint16_t, const uint16_t); // Fill a line using bitwise AND.
    bool solveLine(const uint16_t, const uint16_t); // Solve a line, return solubility.
    bool inferLine(const uint16_t, const uint16_t); // Fix a line arbitrarily, return solubility.

public:
    static bool solve(std::istream&); // Input the clue from istream, return solubility.
    static bool isValid(void); // Return validity.
    static bool isSolved(void); // Return solubility.
    static void print(std::ostream&); // Print the table.
};

#endif // PJHALPHA_NONOGRAM_H
