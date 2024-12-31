#ifndef LINE_CLUE_HPP
#define LINE_CLUE_HPP

#include "nonogram.hpp"

namespace Nonogram {
    /**
     * @class LineClue
     * 
     * @brief Clue of a single line of the nonogram.
     */
    class LineClue {
    public:
        /**
         * @brief Retrieve the size of the line.
         *
         * @return The size of the line.
         */
        ng_size_t getLineSize(void) const;
        /**
         * @brief Retrieve the size of the clue.
         *
         * @return The size of the clue.
         */
        ng_size_t getSize(void) const;
        /**
         * @brief Retrieve the offset (prefix sum) for a clue element.
         *
         * @param cele_i Index of a clue element.
         *
         * @return The offset for a clue element.
         */
        ng_size_t getOffset(const ng_size_t cele_i) const;
        /**
         * @brief Retrieve the margin of the line based on the clue.
         *
         * @return The margin of the line.
         */
        ng_size_t getMargin(void) const;
        /**
         * @brief Set the member variables and validate whether the clue is proper.
         *
         * @param line_size The size of the line.
         * @param size The size of the clue.
         * @param clue_arr The clue array.
         *
         * @return `true` if the clue is valid, `false` otherwise.
         */
        bool init(const ng_size_t line_size, const ng_size_t size, const ng_size_t* const clue_arr);

    private:
        /**
         * @brief The size of the line.
         */
        ng_size_t line_size_;
        /**
         * @brief The size of the clue.
         */
        ng_size_t size_;
        /**
         * @brief The offset (prefix sum) for a clue element.
         *
         * - `dim0`: Index of a clue element.
         */
        ng_size_t offset_[MAX_CLUE_SIZE + 1];
        /**
         * @brief The margin of the line.
         */
        ng_size_t margin_;
    };
}

#include "line_clue.cpp"

#endif // LINE_CLUE_HPP
