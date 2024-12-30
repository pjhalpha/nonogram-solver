#include "clue.hpp"

namespace Nonogram {
    ng_size_t Clue::getSize(const bool vec) const {
        return size_[vec];
    }
    const LineClue &Clue::getLineClue(const bool vec, const ng_size_t vec_i) const {
        return line_clue_[vec][vec_i];
    }
    bool Clue::init(const ng_size_t* const clue_arr) {
        ng_sq_size_t i{0};
        ng_sq_size_t clue_sum[2]{0};

        // Get the number of rows and columns of the table.
        std::memcpy(size_, clue_arr, sizeof(ng_size_t) * 2);
        i += 2;
        
        if (size_[0] > MAX_SIZE || size_[1] > MAX_SIZE) {
            return false;
        }

        // Initialize line_clue_ based on the clue.
        for (ng_size_t vec{0}; vec < 2; ++vec) {
            for (ng_size_t vec_i{0}; vec_i < size_[vec]; ++vec_i) {
                if (!line_clue_[vec][vec_i].init(size_[!vec], clue_arr[i], clue_arr + i + 1)) {
                    return false;
                }

                clue_sum[vec] += line_clue_[vec][vec_i].getOffset(clue_arr[i]) - clue_arr[i];
                i += clue_arr[i] + 1;
            }
        }
        
        if (clue_sum[0] != clue_sum[1]) {
            return false;
        }

        return true;
    }
};
