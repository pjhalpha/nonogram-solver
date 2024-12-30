#ifndef LINE_SOLVER_HPP
#define LINE_SOLVER_HPP

#include "nonogram.hpp"

namespace Nonogram {
    // Nonogram line solver class.
    class LineSolver {
    public:
        LineSolver(LineSolver*);
        LineSolver(const LineSolver&) = delete;
        LineSolver &operator=(const LineSolver&) = delete;
        ~LineSolver(void) = default;

        // Return a pixel of the line.
        ng_sq_size_t get(const ng_size_t i) const;
        LineSolver *getPrevLink(void) const;
        LineSolver *getNextLink(void) const;
        void setNextLink(LineSolver*);
        // Initialize variables.
        void init(const LineClue &, const ng_size_t*);
        // Solve a line, return solubility.
        bool solve(void);
        // Fix a line arbitrarily, return solubility.
        bool infer(void);

    private:
        // The clue of the line.
        const LineClue *line_clue_;
        LineSolver *prev_link_;
        LineSolver *next_link_;

        // The clue element index.
        ng_size_t cele_i_;
        // Line ([index], 0b10 black, 0b01 white, 0b00 gray).
        ng_bit_array_t line_[getBitArraySize(MAX_SIZE + 1, 2)];
        // Line for sweeping ([0 white, 1 black][index]).
        ng_size_t sweep_line_[2][MAX_SIZE + 1];
        // Prefix sum of pixels ([0 white, 1 black][index]).
        ng_size_t color_[2][MAX_SIZE + 2];
        // Shift index ([0 current, 1 previous][clue element index]).
        ng_size_t shift_[2][MAX_CLUE_SIZE + 2];
        // Solubility according to clue element index ([clue element index]).
        ng_bit_array_t cele_cache_[getBitArraySize(MAX_CLUE_SIZE + 2, 1)];
        // Solubility according to shift index ([0 impossible, 1 possible][clue element index][shift index]).
        ng_bit_array_t shift_cache_[2][MAX_CLUE_SIZE + 2][getBitArraySize(MAX_SIZE, 1)];

        // Set a pixel of the line.
        void set(const ng_size_t i, const ng_size_t val);
        // Fill the line for sweeping.
        void fill(const bool color, const ng_size_t start, const ng_size_t end);
        // Sweep sweep_line to fill line.
        void sweep(void);
    };
}

#include "line_solver.cpp"

#endif // LINE_SOLVER_HPP
