#ifndef SOLVER_HPP_
#define SOLVER_HPP_

#include "nonogram.hpp"

namespace Nonogram {
    /**
     * @class LineSolver
     * 
     * @brief Solver for a nonogram.
     */
    class Solver {
    public:
        Solver(void) = default;
        Solver(const Solver&) = delete;
        Solver &operator=(const Solver&) = delete;
        virtual ~Solver(void) = default;

        /**
         * @brief Retrieve an element of the table.
         *
         * @param row The row index of the table.
         * @param col The column index of the table.
         *
         * @return The value of the table.
         */
        virtual ng_size_t get(const ng_size_t row, const ng_size_t col) const final;
        /**
         * @brief Retrieve an element of the table.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         * @param cross_i The index within the vector.
         *
         * @return The value of the table.
         */
        virtual ng_size_t get(const bool vec, const ng_size_t vec_i, const ng_size_t cross_i) const final;
        /**
         * @brief Retrieve the count of remain elements of the table.
         *
         * @return The count of remain elements of the table.
         */
        virtual ng_sq_size_t getRemain(void) const final;
        /**
         * @brief Retrieve the count of soulutions.
         *
         * @return The count of soulutions.
         */
        virtual ng_sq_size_t getCount(void) const final;
        /**
         * @brief Retrieve the time taken to solving the nonogram.
         *
         * @return The time taken to solving the nonogram.
         */
        virtual double getTime(void) const final;
        /**
         * @brief Determine the solvability of a nonogram based on `clue`.
         *
         * @param clue The clue of the nonogram to solve.
         *
         * @return `true` if the clue can be solved, `false` otherwise.
         */
        virtual bool solve(const Clue &clue) final;
        /**
         * @brief Determine whether to continue searching for more solutions after the nonogram is solved.
         *
         * @return `true` if more solutions should be found, `false` otherwise.
         */
        virtual bool wrap(void);
        /**
         * @brief Determine whether to continue searching for more solutions after a line is solved.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         * @param is_inference `true` if called within `Solver::infer`, `false` otherwise.
         *
         * @return `true` if more solutions should be found, `false` otherwise.
         */
        virtual bool inspect(const bool vec, const ng_size_t vec_i, const bool is_inference);

    private:
        /**
         * @brief The clue of the nonogram.
         */
        const Clue *clue_;
        /**
         * @brief The line solver of the line.
         */
        LineSolver *line_solver_;

        /**
         * @brief The count of the found solutions.
         */
        ng_sq_size_t count_;
        /**
         * @brief The start time of solving the nonogram.
         */
        clock_t time_start_;
        /**
         * @brief The end time of solving the nonogram.
         */
        clock_t time_end_;

        /**
         * @brief The current depth of inference.
         */
        ng_size_t depth_;
        /**
         * @brief Table.
         *
         * - `dim0`: Row index of the table
         *
         * - `dim0`: Column index of an bit bundle of the table.
         *
         * The values of a unit bit in the array are `0b10` for black, `0b01` for white, `0b00` for gray,
         * or an error for any other value.
         */
        ng_bit_array_t table_[MAX_SIZE][getBitArraySize(MAX_SIZE, 2)];
        /**
         * @brief The table of depth.
         */
        ng_size_t depth_table_[MAX_SIZE][MAX_SIZE];
        /**
         * @brief A number of grays in the table.
         */
        ng_sq_size_t remain_;
        /**
         * @brief A number of lines can be updated.
         */
        ng_sq_size_t state_;
        /**
         * @brief A number of grays in a line.
         *
         * - `dim0`: 0 for row vectors, 1 for column vectors.
         *
         * - `dim1`: The index of the vector.
         */
        ng_size_t line_remain_[2][MAX_SIZE];
        /**
         * @brief The flag whether a line can be updated.
         *
         * - `dim0`: 0 for row vectors, 1 for column vectors.
         *
         * - `dim1`: Index of an bit bundle of the vector.
         */
        ng_bit_array_t line_state_[2][getBitArraySize(MAX_SIZE, 1)];

        /**
         * @brief Retrieves the line of the table.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         *
         * @return The line of the table.
         */
        const ng_size_t *getLine(const bool vec, const ng_size_t vec_i);
        /**
         * @brief Set an element of the table.
         *
         * @param row The row index of the table.
         * @param col The column index of the table.
         * @param val The value to set.
         */
        void set(const ng_size_t row, const ng_size_t col, const ng_size_t val);
        /**
         * @brief Set an element of the table.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         * @param cross_i The index within the vector.
         * @param val The value to set.
         */
        void set(bool vec, const ng_size_t vec_i, const ng_size_t cross_i, const ng_size_t val);
        /**
         * @brief Fill the table.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         * @param start The starting index where within the vector `val` will be added.
         * @param end The ending index within the vector where `val` will be subtracted.
         * @param val The value to fill.
         */
        void fill(const bool vec, const ng_size_t vec_i, const ng_size_t start, const ng_size_t end, const ng_size_t val);
        /**
         * @brief Initialize member variables and fastly fill some elements of the table.
         *
         * @return `true` if the clue can be solved, `false` otherwise.
         */
        bool init(void);
        /**
         * @brief Solve the nonogram until no further updates can be made to the table.
         *
         * @return `true` if the clue can be solved, `false` otherwise.
         */
        bool solve(void);
        /**
         * @brief Arbitrarily fix a line when no further updates can be made to the table.
         *
         * @return `true` if the clue can be solved, `false` otherwise.
         */
        bool infer(void);
        /**
         * @brief Merge the table with the solved line.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         */
        void merge(const bool vec, const ng_size_t vec_i);
        /**
         * @brief Create a new branch after the line.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         *
         * @return `true` if the clue is solved, `false` otherwise.
         *
         * `LineSolver::solve` or `LineSolver::infer` must be called before use.
         */
        bool next(const bool vec, const ng_size_t vec_i);
        /**
         * @brief Verify the found solution.
         *
         * @return `true` if the found solution is valid, `false` otherwise.
         */
        bool verify(void);
    };
}

#include "solver.cpp"

#endif // SOLVER_HPP
