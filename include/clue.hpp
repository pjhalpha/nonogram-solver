#ifndef CLUE_HPP
#define CLUE_HPP

#include "nonogram.hpp"

namespace Nonogram {
    /**
     * @class Clue
     * 
     * @brief Clue of a nonogram.
     */
    class Clue {
    public:
        /**
         * @brief Retrieve the size of the table.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         *
         * @return The size of the row or column.
         */
        ng_size_t getSize(const bool vec) const;
        /**
         * @brief Retrieve size of the table.
         *
         * @param vec Direction of the clue, 0 for row vectors, 1 for column vectors.
         * @param vec_i Index of the row vectors or column vectors.
         *
         * @return The clue of the line.
         */
        const LineClue &getLineClue(const bool vec, const ng_size_t vec_i) const;
        /**
         * @brief Set the member variables based on the array and validate its structure.
         *
         * @param clue_arr The clue array.
         * First two elements represent the number of rows and columns of the table.
         * Following these, blocks for each row and column are repeated.
         * Each block starts with the size of the clue for that row or column,
         * followed by the clue values themselves.
         *
         * @return `true` if the clue is valid, `false` otherwise.
         */
        bool init(const ng_size_t* const clue_arr);

    private:
        /**
         * @brief Size of the table.
         *
         * - `dim0`: 0 for the row size, 1 for the column size.
         */
        ng_size_t size_[2];
        /**
         * @brief The clues of lines.
         *
         * - `dim0`: 0 for row vector clues, 1 for column vector clues.
         *
         * - `dim1`: The index of a vector.
         */
        LineClue line_clue_[2][MAX_SIZE];
    };
}

#include "clue.cpp"

#endif // CLUE_HPP
