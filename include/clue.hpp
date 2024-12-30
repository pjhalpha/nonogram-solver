#ifndef CLUE_HPP
#define CLUE_HPP

#include "nonogram.hpp"

namespace Nonogram {
    // Nonogram clue class.
    class Clue {
    public:
        // Get the size of the table.
        ng_size_t getSize(const bool) const;
        // Get a line clue of the table.
        const LineClue &getLineClue(const bool, const ng_size_t) const;
        // Input the clue from array and validate, return validity.
        bool init(const ng_size_t* const);

    private:
        // Size of the table ([vector]).
        ng_size_t size_[2];
        // The clues of lines ([vector][vector index]).
        LineClue line_clue_[2][MAX_SIZE];
    };
}

#include "clue.cpp"

#endif // CLUE_HPP
