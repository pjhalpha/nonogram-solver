#ifndef SOLVER_HPP
#define SOLVER_HPP

#include "nonogram.hpp"

namespace Nonogram {
    // Nonogram solver class.
    class Solver {
    public:
        Solver(void) = default;
        Solver(const Solver&) = delete;
        Solver &operator=(const Solver&) = delete;
        virtual ~Solver(void) = default;

        // Return a pixel of the table.
        virtual ng_size_t get(const ng_size_t, const ng_size_t) const final;
        // Return a pixel of the table.
        virtual ng_size_t get(const bool, const ng_size_t, const ng_size_t) const final;
        // Return the count of remain pixels of the table.
        virtual ng_sq_size_t getRemain(void) const final;
        // Return the count of soulutions.
        virtual ng_sq_size_t getCount(void) const final;
        // Return taken time for solving the nonogram.
        virtual double getTime(void) const final;
        // Input the clue from array, return solubility.
        virtual bool solve(const Clue&) final;
        // Whenever a line is solved, return whether to continue finding more solutions.
        virtual bool inspect(const bool, const ng_size_t, const bool);
        // Whenever the nonogram is solved, return whether to continue finding more solutions.
        virtual bool wrap(void);

    private:
        const Clue *clue_;
        LineSolver *line_solver_;

        // Count of the found solutions of the nonogram.
        ng_sq_size_t count_;
        // Time of starting solving the nonogram.
        clock_t time_start_;
        // Time of solving the nonogram.
        clock_t time_end_;

        // Depth of inference.
        ng_size_t depth_;
        // Table ([row][column], 0b10 black, 0b01 white, 0b00 gray).
        ng_bit_array_t table_[MAX_SIZE][getBitArraySize(MAX_SIZE, 2)];
        // Table of depth ([row][column]).
        ng_size_t depth_table_[MAX_SIZE][MAX_SIZE];
        // Number of grays in the table.
        ng_sq_size_t remain_;
        // Number of lines having possibility to be updated.
        ng_sq_size_t state_;
        // Number of grays in line ([vector][vector index]).
        ng_size_t line_remain_[2][MAX_SIZE];
        // Possibility to be updated ([vector][vector index]).
        ng_bit_array_t line_state_[2][getBitArraySize(MAX_SIZE, 1)];

        const ng_size_t *getLine(const bool vec, const ng_size_t vec_i);
        void set(const ng_size_t, const ng_size_t, const ng_size_t);
        // Set a pixel of the table.
        void set(const bool, const ng_size_t, const ng_size_t, const ng_size_t);
        // Fill the table.
        void fill(const bool vec, const ng_size_t vec_i, const ng_size_t start, const ng_size_t end, const ng_size_t val);
        // Initialize and fastly fill some pixles of the table, return solvability.
        bool init(void);
        // Solve nonogram until the table is no longer updated, return whether to stop finding more solutions.
        bool solve(void);
        // Fix one line arbitrarily when the table is no longer updated, return whether to stop finding more solutions.
        bool infer(void);
        // Merge the table with a solved line.
        void merge(const bool, const ng_size_t);
        // Create a new branch from the previous table.
        bool next(const bool, const ng_size_t);
        // Verify the found solution, return correctness.
        bool verify(void);
    };
}

#include "solver.cpp"

#endif // SOLVER_HPP
