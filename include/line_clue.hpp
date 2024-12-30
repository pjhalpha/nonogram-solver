#ifndef LINE_CLUE_HPP
#define LINE_CLUE_HPP

#include "nonogram.hpp"

namespace Nonogram {
    // Nonogram clue of a line class.
    class LineClue {
    public:
        // Get the size of the line.
        ng_size_t getLineSize(void) const;
        // Get the size of the clue.
        ng_size_t getSize(void) const;
        // Get the offset (prefix sum) for a clue element.
        ng_size_t getOffset(const ng_size_t) const;
        // Get the margin of the line based on the clue.
        ng_size_t getMargin(void) const;
        // Set the member variables and validate whether the clue is proper.
        bool init(const ng_size_t, const ng_size_t, const ng_size_t* const);

    private:
        // The size of the line.
        ng_size_t line_size_;
        // The size of the clue.
        ng_size_t size_;
        // The offset (prefix sum) for a clue element ([clue element index]).
        ng_size_t offset_[MAX_CLUE_SIZE + 1];
        // The margin of the line.
        ng_size_t margin_;
    };
}

#include "line_clue.cpp"

#endif // LINE_CLUE_HPP
