#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdint>

namespace Nonogram {
    typedef uint16_t ng_size_t;
    typedef uint32_t ng_sq_size_t;
    typedef uint32_t ng_bit_array_t;

    constexpr ng_size_t MAX_SIZE{255}; // Maximum table size (MAX_SIZE by MAX_SIZE matrix).
    constexpr ng_size_t MAX_CLUE_SIZE{MAX_SIZE + 1 >> 1}; // Maximum clue size.
    constexpr ng_size_t BIT_SIZE{sizeof(ng_bit_array_t) << 3};
    constexpr ng_size_t getBitArraySize(const ng_size_t size, const ng_size_t one_bit) {
        if (one_bit == 1) return (size - 1) / BIT_SIZE + 1;
        else if (one_bit == 2) return (size - 1) / (BIT_SIZE >> 1) + 1;
        else throw;
    }
    inline constexpr ng_size_t getBitArrayElement(const ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t one_bit) {
        if (one_bit == 1) return bit_array[i >> 5] >> (i & 31) & 0b1;
        else if (one_bit == 2) return bit_array[i >> 4] >> ((i & 15) << 1) & 0b11;
        else throw;
    }
    inline constexpr void andBitArrayElement(ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t v, const ng_size_t one_bit) {
        if (one_bit == 1) bit_array[i >> 5] &= ~((v ^ 0b1) << (i & 31));
        else if (one_bit == 2) bit_array[i >> 4] &= ~((v ^ 0b11) << ((i & 15) << 1));
        else throw; 
    }
    inline constexpr void orBitArrayElement(ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t v, const ng_size_t one_bit) {
        if (one_bit == 1) bit_array[i >> 5] |= v << (i & 31);
        else if (one_bit == 2) bit_array[i >> 4] |= v << ((i & 15) << 1);
        else throw; 
    }
    
    class Solver;
    class LineSolver;
    
    class Clue { // Nonogram clue class
        friend Solver;
        friend LineSolver;

    private:
        Clue(void) = default;
        Clue(const Clue&) = delete;
        Clue &operator=(const Clue&) = delete;
        
        ng_size_t size[2]; // Size of the table ([vector(0 row, 1 column)]).
        ng_size_t offset[2][MAX_SIZE][MAX_CLUE_SIZE + 1]; // Prefix sum of the clue ([vector][vector index][clue index]).
        ng_size_t clue_size[2][MAX_SIZE]; // Size of the clue ([vector][vector index]).
        ng_size_t margin[2][MAX_SIZE]; // Margin based on size of the table and the clue ([vector][vector index]).
        bool input(const ng_size_t* const); // Input the clue from array and validate, return validity.
    };
    
    class Solver { // Nonogram solver class
        friend LineSolver;

    public:
        Solver(void);
        Solver(const Solver&) = delete;
        Solver &operator=(const Solver&) = delete;
        
        void setWrap(bool (*)(void) = nullptr); // Specify a function to be called whenever a solution is found.
        void setInspect(void (*)(const ng_size_t, const ng_size_t, const bool) = nullptr); // Specify a function to be called when a line is solved.
        bool solve(const ng_size_t* const); // Input the clue from array, return solubility.
        ng_size_t get(const ng_size_t, const ng_size_t) const; // Return a pixel of the table.
        ng_size_t get(const bool, const ng_size_t, const ng_size_t) const; // Return a pixel of the table.
        ng_sq_size_t getRemain(void) const; // Return the count of remain pixels of the table.
        ng_size_t getCount(void) const; // Return the count of soulutions.
        long double getTime(void) const; // Return taken time for solving the nonogram.
    
    private:
        Clue clue; // Clue.
        LineSolver *line; // Line solver.
        ng_size_t count; // Count of the found solutions of the nonogram.
        clock_t time_start; // Time of starting solving the nonogram.
        clock_t time_end; // Time of solving the nonogram.
        bool (*wrap)(void); // Function to be called whenever the nonogram is solved, return whether to continue finding more solutions.
        void (*inspect)(const ng_size_t, const ng_size_t, const bool); // Function to be called whenever a line is solved.
        
        ng_size_t depth; // Depth of inference.
        ng_bit_array_t table[MAX_SIZE][getBitArraySize(MAX_SIZE, 2)]; // Table ([row][column], 0b10 black, 0b01 white, 0b00 gray).
        ng_size_t depth_table[MAX_SIZE][MAX_SIZE]; // Table of depth ([row][column]).
        ng_sq_size_t remain; // Number of grays in the table.
        ng_sq_size_t state; // Number of lines having possibility to be updated.
        ng_size_t line_remain[2][MAX_SIZE]; // Number of grays in line ([vector][vector index]).
        ng_bit_array_t line_state[2][getBitArraySize(MAX_SIZE, 1)]; // Possibility to be updated ([vector][vector index]).
        bool init(void); // Initialize and fastly fill some pixles of the table, return solvability.
        bool solve(void); // Solve nonogram until the table is no longer updated, return whether to stop finding more solutions.
        bool infer(void); // Fix one line arbitrarily when the table is no longer updated, return whether to stop finding more solutions.
        void merge(const ng_size_t, const ng_size_t); // Merge the table and a solved line.
        bool next(const ng_size_t, const ng_size_t); // Create a new branch from the previous table.
        bool verify(void); // Verify the found solution, return correctness.
        void set(const ng_size_t, const ng_size_t, const ng_size_t); // Set a pixel of the table.
        void set(const bool, const ng_size_t, const ng_size_t, const ng_size_t); // Set a pixel of the table.
        void fill(const bool, const ng_size_t, const ng_size_t, const ng_size_t, const ng_size_t); // Fill the table.
    };
    
    class LineSolver { // Nonogram line solver class
        friend Solver;

    private:
        LineSolver(Solver&, LineSolver*);
        LineSolver(const LineSolver&) = delete;
        LineSolver &operator=(const LineSolver&) = delete;
        
        const Clue &clue; // Clue.
        Solver &solver; // Solver.
        LineSolver *llink, *rlink; // Links of a line solver for inference.
        ng_bit_array_t line[getBitArraySize(MAX_SIZE + 1, 2)]; // Line ([index], 0b10 black, 0b01 white, 0b00 gray).
        ng_size_t sweep_line[2][MAX_SIZE + 1]; // Line for sweeping ([0 white, 1 black][index]).
        ng_size_t color[2][MAX_SIZE + 2]; // Prefix sum of pixels ([0 white, 1 black][index]).
        ng_size_t shift[2][MAX_CLUE_SIZE + 2]; // Shift index ([0 current, 1 previous][clue index]).
        ng_bit_array_t clue_cache[getBitArraySize(MAX_CLUE_SIZE + 2, 1)]; // Solubility according to clue index ([clue index]).
        ng_bit_array_t shift_cache[2][MAX_CLUE_SIZE + 2][getBitArraySize(MAX_SIZE, 1)]; // Solubility according to shift index ([0 impossible, 1 possible][clue index][shift index]).
        void init(const ng_size_t, const ng_size_t); // Initialize variables.
        bool solve(const ng_size_t, const ng_size_t); // Solve a line, return solubility.
        bool infer(const ng_size_t, const ng_size_t); // Fix a line arbitrarily, return solubility.
        ng_sq_size_t get(const ng_size_t) const; // Return a pixel of the line.
        void set(const ng_size_t, const ng_size_t); // Set a pixel of the line.
        void fill(const ng_size_t, const ng_size_t, const bool); // Fill the line for sweeping.
        void sweep(const ng_size_t); // Sweep sweep_line to fill line.
    };
};

#endif // NONOGRAM_H
