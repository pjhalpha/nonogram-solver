#include "nonogram.hpp"

namespace Nonogram {
    bool Clue::input(const ng_size_t* const input_arr) {
        ng_sq_size_t i{0};
        ng_sq_size_t clue_sum[2]{0};

        // Get the number of rows and columns of the table.
        std::memcpy(size_, input_arr, sizeof(ng_size_t) * 2);
        i += 2;
        
        if (size_[0] > MAX_SIZE || size_[1] > MAX_SIZE) {
            return false;
        }

        // Initialize size_, offset_, clue_size_ and margin_ from the clue.
        for (ng_size_t vec{0}; vec < 2; ++vec) for (ng_size_t vec_i{0}; vec_i < size_[vec]; ++vec_i) {
            clue_size_[vec][vec_i] = input_arr[i++];

            if (clue_size_[vec][vec_i] == 0 || clue_size_[vec][vec_i] >= MAX_CLUE_SIZE) {
                return false;
            }

            std::memcpy(offset_[vec][vec_i] + 1, input_arr + i, sizeof(ng_size_t) * clue_size_[vec][vec_i]);
            i += clue_size_[vec][vec_i];
            for (ng_size_t cele_i{0}; cele_i < clue_size_[vec][vec_i]; ++cele_i) {
                offset_[vec][vec_i][cele_i + 1] += offset_[vec][vec_i][cele_i] + 1;
            }

            if (clue_size_[vec][vec_i] != 1 && offset_[vec][vec_i][1] == 0) {
                return false;
            }

            margin_[vec][vec_i] = size_[!vec] - offset_[vec][vec_i][clue_size_[vec][vec_i]] + 1;
            clue_sum[vec] += offset_[vec][vec_i][clue_size_[vec][vec_i]] - clue_size_[vec][vec_i];
        
            if (margin_[vec][vec_i] < 0) {
                return false;
            }
        }
        return clue_sum[0] == clue_sum[1];
    }
};
