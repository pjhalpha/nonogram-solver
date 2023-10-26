#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <iostream>
#include <cstring>
#include <ctime>

namespace Nonogram {    
    constexpr uint16_t MAX_SIZE{255}; // Maximum table size (MAX_SIZE by MAX_SIZE matrix).
    constexpr uint16_t MAX_CLUE_SIZE{MAX_SIZE + 1 >> 1}; // Maximum clue size.
    constexpr uint16_t getArraySize(const uint16_t size, const uint16_t bucket_size) {
        return (size - 1) / bucket_size + 1;
    }
    
    class Solver {
    private:
        Solver(void) = default;
        Solver(const Solver*, const uint16_t, const uint16_t); // Create a new branch from the previous table.
        ~Solver(void) = default;
        Solver(const Solver&) = delete;
        Solver &operator=(const Solver&) = delete;
        static Solver &getInstance(void); // Return the singleton instance.

        inline static Solver &base{getInstance()}; // Singleton instance.
        inline static uint16_t size[2]; // Size of the table ([vector(0 row, 1 column)]).
        inline static uint16_t offset[2][MAX_SIZE][MAX_CLUE_SIZE + 1]; // Prefix sum of the clue ([vector][vector index][clue index]).
        inline static uint16_t clue_size[2][MAX_SIZE]; // Size of the clue ([vector][vector index]).
        inline static uint16_t margin[2][MAX_SIZE]; // Margin based on size of the table and the clue ([vector][vector index]).
        inline static uint16_t count; // Count of the solutions of the nonogram.
        inline static clock_t time_start; // Time of starting solving the nonogram.
        inline static clock_t time_end; // Time of solving the nonogram.
        inline static void (*inspect)(const Solver&, const uint16_t, const uint16_t, const bool); // Operate whenever a line is solved.
        inline static bool (*wrap)(const Solver&); // Operate when the nonogram is solved, return whether to stop finding more solutions.
        static bool input(const uint16_t*); // Input the clue from istream and validate, return validity.

        void merge(const uint16_t, const uint16_t); // Merge the table and a solved line.
        bool solve(void); // Solve nonogram until the table is no longer updated, return whether to stop finding more solutions.
        bool infer(void); // Fix one line arbitrarily when the table is no longer updated, return whether to stop finding more solutions.
        bool verify(void); // Verify the found solution, return correctness.

        uint32_t table[MAX_SIZE][getArraySize(MAX_SIZE, 16)]; // Table ([row][column], 0b10 black, 0b01 white, 0b00 gray).
        uint32_t remain; // Number of grays in the table.
        uint32_t state; // Number of lines having possibility to be updated.
        uint16_t line_remain[2][MAX_SIZE]; // Number of grays in line ([vector][vector index]).
        uint32_t line_state[2][getArraySize(MAX_SIZE, 32)]; // Possibility to be updated ([vector][vector index]).
        void set(const uint16_t, const uint16_t, const uint16_t); // Set a pixel of the table.
        void set(const bool, const uint16_t, const uint16_t, const uint16_t); // Set a pixel of the table.
        void fill(const bool, const uint16_t, const uint16_t, const uint16_t, const uint16_t); // Fill the table.
        void init(void); // Initialize and quickly fill in some pixles of the table.
        void copy(const Solver*); // Copy the table.

        uint32_t line[getArraySize(MAX_SIZE + 1, 16)]; // Line ([index], 0b10 black, 0b01 white, 0b00 gray).
        uint16_t color[2][MAX_SIZE + 2]; // Prefix sum of pixels ([0 white, 1 black][index]).
        uint16_t shift[2][MAX_CLUE_SIZE + 2]; // Shift index ([0 current, 1 previous][clue index]).
        uint32_t cache[2][MAX_CLUE_SIZE + 2][getArraySize(MAX_SIZE, 32)]; // Solubility according to shift index ([0 impossible, 1 possible][clue index][shift index]).
        uint32_t subcache[getArraySize(MAX_CLUE_SIZE + 2, 32)]; // Solubility according to clue index ([clue index]).
        void initLine(const uint16_t, const uint16_t); // Initialize variables.
        void fillLine(const uint16_t, const uint16_t, const uint16_t); // Fill a line using bitwise AND.
        bool solveLine(const uint16_t, const uint16_t); // Solve a line, return solubility.
        bool inferLine(const uint16_t, const uint16_t); // Fix a line arbitrarily, return solubility.

    public:
        static bool solve(const uint16_t* const, void (*)(const Solver&, const uint16_t, const uint16_t, const bool), bool (*)(const Solver&)); // Input the clue from istream, return solubility.
        static uint16_t getCount(void);
        static double getTime(void); // Return taken time for solving the nonogram.

        uint16_t get(const uint16_t, const uint16_t) const; // Get a pixel of the table.
        uint16_t get(const bool, const uint16_t, const uint16_t) const; // Get a pixel of the table.
        uint32_t getRemain(void) const; // Get the count of remain pixels of the table.
    };
};

#endif // NONOGRAM_H
