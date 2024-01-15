#include "nonogram.hpp"

namespace Nonogram {
    bool Clue::input(const ng_size_t* const input_arr) {
        ng_sq_size_t ii{0};
        ng_sq_size_t clue_sum[2]{0};

        // Get the number of rows and columns of the table.
        memcpy(size, input_arr, sizeof(ng_size_t) * 2);
        ii += 2;
        
        if (size[0] > MAX_SIZE || size[1] > MAX_SIZE) return false;

        // Initialize size, offset, clue_size and margin from the clue.
        for (ng_size_t vec{0}; vec < 2; ++vec) for (ng_size_t vi{0}; vi < size[vec]; ++vi) {
            clue_size[vec][vi] = input_arr[ii++];

            if (!clue_size[vec][vi] || clue_size[vec][vi] >= MAX_CLUE_SIZE) return false;

            memcpy(offset[vec][vi] + 1, input_arr + ii, sizeof(ng_size_t) * clue_size[vec][vi]);
            ii += clue_size[vec][vi];
            for (ng_size_t ci{0}; ci < clue_size[vec][vi]; ++ci) {
                offset[vec][vi][ci + 1] += offset[vec][vi][ci] + 1;
            }

            if (clue_size[vec][vi] != 1 && !offset[vec][vi][1]) return false;

            margin[vec][vi] = size[!vec] - offset[vec][vi][clue_size[vec][vi]] + 1;
            clue_sum[vec] += offset[vec][vi][clue_size[vec][vi]] - clue_size[vec][vi];
        
            if (margin[vec][vi] < 0) return false;
        }
        return clue_sum[0] == clue_sum[1];
    }
};
