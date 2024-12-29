#include "nonogram.hpp"

namespace Nonogram {
    LineSolver::LineSolver(Solver &solver, LineSolver *prev_link) : solver_{solver}, clue_{solver.clue_}, prev_link_{prev_link}, next_link_{nullptr} {}

    void LineSolver::init(const bool vec, const ng_size_t vec_i) {
        // Initialize color_.
        for (ng_size_t cross_i{0}; cross_i < clue_.size_[!vec]; ++cross_i) {
            color_[0][cross_i + 1] = color_[0][cross_i] + (solver_.get(vec, vec_i, cross_i) == 0b01);
            color_[1][cross_i + 1] = color_[1][cross_i] + (solver_.get(vec, vec_i, cross_i) == 0b10);
        }
        color_[0][clue_.size_[!vec] + 1] = color_[0][clue_.size_[!vec]];
        color_[1][clue_.size_[!vec] + 1] = color_[1][clue_.size_[!vec]];

        // Initialize shift_.
        shift_[0][0] = shift_[0][1] = 0;
        std::memset(shift_[1], 0, sizeof shift_[1]);

        // Initialize cele_cache_ and shift_cache_.
        std::memset(shift_cache_[true][0], -1, sizeof shift_cache_[true][0]);
        std::memset(shift_cache_[true][clue_.clue_size_[vec][vec_i] + 1], 0, sizeof shift_cache_[true][clue_.clue_size_[vec][vec_i] + 1]);
        for (ng_size_t cele_i{1}; cele_i <= clue_.clue_size_[vec][vec_i]; ++cele_i) {
            std::memset(shift_cache_[true][cele_i], 0, sizeof shift_cache_[true][cele_i]);
            std::memset(shift_cache_[false][cele_i], 0, sizeof shift_cache_[false][cele_i]);
            for (ng_size_t shift_i{0}; shift_i <= clue_.margin_[vec][vec_i]; ++shift_i) {
                if (color_[0][clue_.offset_[vec][vec_i][cele_i - 1] + shift_i] != color_[0][clue_.offset_[vec][vec_i][cele_i] + shift_i - 1] || color_[1][clue_.offset_[vec][vec_i][cele_i] + shift_i - 1] != color_[1][clue_.offset_[vec][vec_i][cele_i] + shift_i]) {
                    orBitArrayElement(shift_cache_[false][cele_i], shift_i, 0b1, 1);
                }
            }
        }
        for (ng_size_t shift_i{0}; shift_i <= clue_.margin_[vec][vec_i]; ++shift_i) {
            if (color_[1][clue_.offset_[vec][vec_i][clue_.clue_size_[vec][vec_i]] + shift_i - 1] == color_[1][clue_.size_[!vec]]) {
                for (; shift_i <= clue_.margin_[vec][vec_i]; ++shift_i) {
                    orBitArrayElement(shift_cache_[true][clue_.clue_size_[vec][vec_i] + 1], shift_i, 0b1, 1);
                }
            } else {
                orBitArrayElement(shift_cache_[false][clue_.clue_size_[vec][vec_i]], shift_i, 0b1, 1);
            }
        }
        std::memset(cele_cache_, 0, sizeof cele_cache_);
    }
    bool LineSolver::solve(const bool vec, const ng_size_t vec_i) {
        init(vec, vec_i);
        std::memset(sweep_line_, 0, sizeof sweep_line_);
        for (ng_size_t cele_i{1}; cele_i > 0;) {
            for (ng_size_t &shift_i{shift_[0][cele_i]}; shift_i <= clue_.margin_[vec][vec_i]; ++shift_i) {
                // Proceed to next shift index if current one is impossible.
                if (getBitArrayElement(shift_cache_[false][cele_i], shift_i, 1) != 0b0) {
                    continue;
                }

                // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
                if (color_[1][clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i - 1]] != color_[1][clue_.offset_[vec][vec_i][cele_i - 1] + shift_i]) {
                    shift_i = clue_.margin_[vec][vec_i] + 1;

                    break;
                }
                
                // Ignore current clue element index if current shift index is known as a solution.
                if (getBitArrayElement(shift_cache_[true][cele_i + 1], shift_[0][cele_i], 1) != 0b0) {
                    ++cele_i;
                    shift_[0][cele_i] = clue_.margin_[vec][vec_i] + 1;
                    orBitArrayElement(cele_cache_, cele_i, 0b1, 1);

                    break;
                }

                // Probe next clue element index if shift index of current one is not probed and not impossible.
                if (color_[0][clue_.offset_[vec][vec_i][cele_i - 1] + shift_i] == color_[0][clue_.offset_[vec][vec_i][cele_i] + shift_i - 1]) {
                    ++cele_i;
                    shift_[0][cele_i] = shift_[0][cele_i - 1];
                    andBitArrayElement(cele_cache_, cele_i, 0b0, 1);

                    break;
                }
            }
            
            // Probe next shift index of previous clue element index if current clue element index is reached to the end of shift index.
            if (shift_[0][cele_i] > clue_.margin_[vec][vec_i]) {
                --cele_i;

                // Set previous shift index cele_cache to true if any solutions are found in current clue element index.
                if (getBitArrayElement(cele_cache_, cele_i + 1, 1) != 0b0) {
                    orBitArrayElement(cele_cache_, cele_i, 0b1, 1);

                    // Set previous shift index to cache and fill the line of current clue element index if previous shift index is not set to cache.
                    // The order of setting a shift index to cache for each clue element index is guaranteed to be in ascending order.
                    if (getBitArrayElement(shift_cache_[true][cele_i], shift_[0][cele_i], 1) == 0b0) {
                        if (shift_[1][cele_i] == 0) {
                            fill(clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i - 1], clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i], false);
                            fill(clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i], clue_.offset_[vec][vec_i][cele_i] + shift_[0][cele_i] - 1, true);
                        } else {
                            fill(std::max(clue_.offset_[vec][vec_i][cele_i - 1] + shift_[1][cele_i], clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i - 1]), clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i], false);
                            fill(std::max(clue_.offset_[vec][vec_i][cele_i] + shift_[1][cele_i] - 1, clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i]), clue_.offset_[vec][vec_i][cele_i] + shift_[0][cele_i] - 1, true);
                        }
                        fill(clue_.offset_[vec][vec_i][cele_i] + shift_[0][cele_i] - 1, clue_.offset_[vec][vec_i][cele_i] + shift_[0][cele_i], false);
                        shift_[1][cele_i] = shift_[0][cele_i];
                        orBitArrayElement(shift_cache_[true][cele_i], shift_[0][cele_i], 0b1, 1);
                    }
                } else {
                    orBitArrayElement(shift_cache_[false][cele_i], shift_[0][cele_i], 0b1, 1);
                }

                // Probe next shift index of previous clue element index.
                ++shift_[0][cele_i];
                
                continue;
            }
        }

        // Check if any solutions are found.
        if (cele_cache_[0] & 1) {
            // Fill the back of the smallest shift index of the last clue element index with 0b01.
            for (ng_size_t shift_i{0};; ++shift_i) {
                if (getBitArrayElement(shift_cache_[true][clue_.clue_size_[vec][vec_i]], shift_i, 1) != 0b0) {
                    fill(clue_.offset_[vec][vec_i][clue_.clue_size_[vec][vec_i]] + shift_i, clue_.size_[!vec], false);
                    sweep(vec);

                    break;
                }
            }

            return true;
        }
        return false;
    }
    bool LineSolver::infer(const bool vec, const ng_size_t vec_i) {
        init(vec, vec_i);
        for (ng_size_t cele_i{1}; cele_i > 0;) {
            for (ng_size_t &shift_i{shift_[0][cele_i]}; shift_i <= clue_.margin_[vec][vec_i]; ++shift_i) {
                // Proceed to next shift index if current one is impossible.
                if (getBitArrayElement(shift_cache_[false][cele_i], shift_i, 1) != 0b0) {
                    continue;
                }
                
                // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
                if (color_[1][clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i - 1]] != color_[1][clue_.offset_[vec][vec_i][cele_i - 1] + shift_i]) {
                    shift_i = clue_.margin_[vec][vec_i] + 1;

                    break;
                }

                // Probe next clue element index if shift index of current one is not probed and not impossible.
                if (color_[0][clue_.offset_[vec][vec_i][cele_i - 1] + shift_i] == color_[0][clue_.offset_[vec][vec_i][cele_i] + shift_i - 1]) {
                    ++cele_i;
                    shift_[0][cele_i] = shift_[0][cele_i - 1];
                    andBitArrayElement(cele_cache_, cele_i, 0b0, 1);

                    break;
                }
            }

            // Probe next shift index of previous clue element index if current clue element index is reached to the end of shift index.
            if (shift_[0][cele_i] > clue_.margin_[vec][vec_i]) {
                --cele_i;

                // Set previous shift index cele_cache to true if any solutions are found in current clue element index.
                if (getBitArrayElement(cele_cache_, cele_i + 1, 1) != 0b0) {
                    orBitArrayElement(cele_cache_, cele_i, 0b1, 1);
                } else {
                    andBitArrayElement(shift_cache_[false][cele_i], shift_[0][cele_i], 0b0, 1);
                }

                // Probe next shift index of previous clue element index.
                ++shift_[0][cele_i];
                
                continue;
            }
            
            if (cele_i == clue_.clue_size_[vec][vec_i] + 1) {
                if (getBitArrayElement(shift_cache_[true][cele_i], shift_[0][cele_i - 1], 1) != 0b0) {
                    // Fill sweep_line and line.
                    std::memset(sweep_line_, 0, sizeof sweep_line_);
                    for (cele_i = 1; cele_i <= clue_.clue_size_[vec][vec_i]; ++cele_i) {
                        fill(clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i - 1], clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i], false);
                        fill(clue_.offset_[vec][vec_i][cele_i - 1] + shift_[0][cele_i], clue_.offset_[vec][vec_i][cele_i] + shift_[0][cele_i] - 1, true);
                        fill(clue_.offset_[vec][vec_i][cele_i] + shift_[0][cele_i] - 1, clue_.offset_[vec][vec_i][cele_i] + shift_[0][cele_i], false);
                    }
                    fill(clue_.offset_[vec][vec_i][clue_.clue_size_[vec][vec_i]] + shift_[0][clue_.clue_size_[vec][vec_i]], clue_.size_[!vec], false);
                    sweep(vec);

                    // Try to solve inferred table if clue element index reached at the last and found a solution.
                    if (solver_.next(vec, vec_i)) {
                        return true;
                    }

                    orBitArrayElement(cele_cache_, cele_i - 1, 0b1, 1);
                }

                // Probe next shift index of previous clue element index.
                --cele_i;
                ++shift_[0][cele_i];
            }
        }
        
        return false;
    }
    ng_sq_size_t LineSolver::get(const ng_size_t i) const {
        return getBitArrayElement(line_, i, 2);
    }
    void LineSolver::set(const ng_size_t i, const ng_size_t val) {
        andBitArrayElement(line_, i, val, 2);
    }
    void LineSolver::fill(const ng_size_t start, const ng_size_t end, const bool val) {
        ++sweep_line_[val][start];
        --sweep_line_[val][end];
    }
    void LineSolver::sweep(const ng_size_t vec) {
        std::memset(line_, -1, sizeof line_);
        for (ng_size_t c{0}; c < 2; ++c) for (ng_size_t vec_i{0}; vec_i < clue_.size_[!vec]; ++vec_i) {
            if (sweep_line_[c][vec_i]) {
                set(vec_i, 0b1 << c);
            }
            sweep_line_[c][vec_i + 1] += sweep_line_[c][vec_i];
        }
    }
};
