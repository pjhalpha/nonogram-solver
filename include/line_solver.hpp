#ifndef LINE_SOLVER_HPP
#define LINE_SOLVER_HPP

#include "nonogram.hpp"

namespace Nonogram {
    /**
     * @class LineSolver
     * 
     * @brief Solver for a single line of the nonogram.
     */
    class LineSolver {
    public:
        /**
         * @brief Constructor for LineSolver.
         * 
         * @param prev_link The previous link of the line solver.
         */
        LineSolver(LineSolver *prev_link);
        LineSolver(const LineSolver&) = delete;
        LineSolver &operator=(const LineSolver&) = delete;
        ~LineSolver(void) = default;

        /**
         * @brief Retrieve an element of the line.
         *
         * @param i Index of the line.
         *
         * @return The value of the line.
         */
        ng_sq_size_t get(const ng_size_t i) const;
        /**
         * @brief Retrieve the previous link of the line solver.
         *
         * @return The previous link of the line solver.
         */
        LineSolver *getPrevLink(void) const;
        /**
         * @brief Retrieve the next link of the line solver.
         *
         * @return The next link of the line solver.
         */
        LineSolver *getNextLink(void) const;
        void setNextLink(LineSolver*);
        /**
         * @brief Initialize all member variables based on `line_clue`.
         *
         * @param line_clue The clue of the line.
         * @param line The current status of the line.
         */
        void init(const LineClue &line_clue, const ng_size_t *line);
        /**
         * @brief Solve a line.
         *
         * @return `true` if the line can be solved, `false` otherwise.
         *
         * `LineSolver::init` must be called before use.
         * A solution is stored in `line_`.
         */
        bool solve(void);
        /**
         * @brief Fix a line arbitrarily.
         *
         * @return `true` if the line can be solved, `false` otherwise.
         *
         * `LineSolver::init` must be called before use.
         * The function will continue returning `true` on each call until a new solution is found,
         * after which it will always return `false` for all subsequent calls.
         * A solution is stored in `line_`.
         */
        bool infer(void);

    private:
        /**
         * @brief The clue of the line.
         */
        const LineClue *line_clue_;
        /**
         * @brief The previous link of the line solver for the inference.
         */
        LineSolver *prev_link_;
        /**
         * @brief The next link of the line solver for the inference.
         */
        LineSolver *next_link_;


        /**
         * @brief The clue element index for `LineSolver::solve` and `LineSolver::infer`.
         */
        ng_size_t cele_i_;
        /**
         * @brief The line.
         *
         * - `dim0`: Index of an bit bundle of the line.
         *
         * The array that the result of solving the line is stored after `solve` or `infer` was called.
         * The values of a unit bit in the array are `0b10` for black, `0b01` for white, `0b00` for gray,
         * or an error for any other value.
         */
        ng_bit_array_t line_[getBitArraySize(MAX_SIZE + 1, 2)];
        /**
         * @brief The array used internally by `LineSolver::solve` to sweep the line and find the reuslt.
         *
         * - `dim0`: 0 for white blocks, 1 for black blocks.
         *
         * - `dim1`: Index of an element of the line, with 1-based indexing.
         */
        ng_size_t sweep_line_[2][MAX_SIZE + 1];
        /**
         * @brief Prefix sum of elements of the line.
         *
         * - `dim0`: 0 for white blocks, 1 for black blocks.
         *
         * - `dim1`: Index of an element of the line, with 1-based indexing.
         */
        ng_size_t color_[2][MAX_SIZE + 2];
        /**
         * @brief The array for store shift indices.
         *
         * - `dim0`: 0 for an impossible index, 1 for a possible index.
         *
         * - `dim1`: Index of a clue element, with 1-based indexing.
         */
        ng_size_t shift_[2][MAX_CLUE_SIZE + 2];
        /**
         * @brief Solubility according to clue element index.
         *
         * - `dim0`: Index of an bit bundle of a clue element, with 1-based indexing..
         */
        ng_bit_array_t cele_cache_[getBitArraySize(MAX_CLUE_SIZE + 2, 1)];
        /**
         * @brief Solubility according to shift index.
         *
         * - `dim0`: 0 for an impossible index, 1 for a possible index.
         *
         * - `dim1`: Index of an bit bundle of a clue element, with 1-based indexing.
         *
         * - `dim2`: The shift index of a clue element, with 1-based indexing.
         */
        ng_bit_array_t shift_cache_[2][MAX_CLUE_SIZE + 2][getBitArraySize(MAX_SIZE, 1)];

        /**
         * @brief Set an element of the line.
         *
         * @param i Index of an element of the line.
         * @param val The value to set.
         */
        void set(const ng_size_t i, const ng_size_t val);
        /**
         * @brief Fill `sweep_line_`.
         *
         * @param color 0 for white blocks, 1 for black blocks.
         * @param start The starting index where 1 will be added.
         * @param end The ending index where 1 will be subtracted.
         */
        void fill(const bool color, const ng_size_t start, const ng_size_t end);
        /**
         * @brief Fill `line_` based on `sweep_line_`.
         */
        void sweep(void);
    };
}

#include "line_solver.cpp"

#endif // LINE_SOLVER_HPP
