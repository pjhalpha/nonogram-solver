#include "line_solver.hpp"

namespace Nonogram {
    LineSolver::LineSolver(LineSolver *prev_link) : prev_link_{prev_link}, next_link_{nullptr} {}

    ng_sq_size_t LineSolver::get(const ng_size_t i) const {
        return getBitArrayElement(line_, i, 2);
    }
    LineSolver *LineSolver::getPrevLink(void) const {
        return prev_link_;
    }
    LineSolver *LineSolver::getNextLink(void) const {
        return next_link_;
    }
    void LineSolver::setNextLink(LineSolver *next_link) {
        next_link_ = next_link;
    }
    void LineSolver::init(const LineClue &line_clue, const ng_size_t *line) {
        line_clue_ = &line_clue;
        cele_i_ = 1;
        
        // Initialize color_.
        color_[0][0] = color_[0][1] = 0;
        for (ng_size_t cross_i{0}; cross_i < line_clue_->getLineSize(); ++cross_i) {
            color_[0][cross_i + 1] = color_[0][cross_i] + (line[cross_i] == 0b01);
            color_[1][cross_i + 1] = color_[1][cross_i] + (line[cross_i] == 0b10);
        }
        color_[0][line_clue_->getLineSize() + 1] = color_[0][line_clue_->getLineSize()];
        color_[1][line_clue_->getLineSize() + 1] = color_[1][line_clue_->getLineSize()];

        // Initialize shift_.
        shift_[0][0] = shift_[0][1] = 0;
        std::memset(shift_[1], 0, sizeof shift_[1]);

        // Initialize cele_cache_ and shift_cache_.
        std::memset(shift_cache_[true][0], -1, sizeof shift_cache_[true][0]);
        std::memset(shift_cache_[true][line_clue_->getSize() + 1], 0, sizeof shift_cache_[true][line_clue_->getSize() + 1]);
        for (ng_size_t cele_i{1}; cele_i <= line_clue_->getSize(); ++cele_i) {
            std::memset(shift_cache_[true][cele_i], 0, sizeof shift_cache_[true][cele_i]);
            std::memset(shift_cache_[false][cele_i], 0, sizeof shift_cache_[false][cele_i]);
            for (ng_size_t shift_i{0}; shift_i <= line_clue_->getMargin(); ++shift_i) {
                if (color_[0][line_clue_->getOffset(cele_i - 1) + shift_i] != color_[0][line_clue_->getOffset(cele_i) + shift_i - 1] || color_[1][line_clue_->getOffset(cele_i) + shift_i - 1] != color_[1][line_clue_->getOffset(cele_i) + shift_i]) {
                    orBitArrayElement(shift_cache_[false][cele_i], shift_i, 0b1, 1);
                }
            }
        }
        for (ng_size_t shift_i{0}; shift_i <= line_clue_->getMargin(); ++shift_i) {
            if (color_[1][line_clue_->getOffset(line_clue_->getSize()) + shift_i - 1] == color_[1][line_clue_->getLineSize()]) {
                for (; shift_i <= line_clue_->getMargin(); ++shift_i) {
                    orBitArrayElement(shift_cache_[true][line_clue_->getSize() + 1], shift_i, 0b1, 1);
                }
            } else {
                orBitArrayElement(shift_cache_[false][line_clue_->getSize()], shift_i, 0b1, 1);
            }
        }
        std::memset(cele_cache_, 0, sizeof cele_cache_);
    }
    bool LineSolver::solve(void) {
        std::memset(sweep_line_, 0, sizeof sweep_line_);
        for (cele_i_ = 1; cele_i_ > 0;) {
            for (ng_size_t &shift_i{shift_[0][cele_i_]}; shift_i <= line_clue_->getMargin(); ++shift_i) {
                // Proceed to next shift index if current one is impossible.
                if (getBitArrayElement(shift_cache_[false][cele_i_], shift_i, 1) != 0b0) {
                    continue;
                }

                // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
                if (color_[1][line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_ - 1]] != color_[1][line_clue_->getOffset(cele_i_ - 1) + shift_i]) {
                    shift_i = line_clue_->getMargin() + 1;

                    break;
                }
                
                // Ignore current clue element index if current shift index is known as a solution.
                if (getBitArrayElement(shift_cache_[true][cele_i_ + 1], shift_[0][cele_i_], 1) != 0b0) {
                    ++cele_i_;
                    shift_[0][cele_i_] = line_clue_->getMargin() + 1;
                    orBitArrayElement(cele_cache_, cele_i_, 0b1, 1);

                    break;
                }

                // Probe next clue element index if shift index of current one is not probed and not impossible.
                if (color_[0][line_clue_->getOffset(cele_i_ - 1) + shift_i] == color_[0][line_clue_->getOffset(cele_i_) + shift_i - 1]) {
                    ++cele_i_;
                    shift_[0][cele_i_] = shift_[0][cele_i_ - 1];
                    andBitArrayElement(cele_cache_, cele_i_, 0b0, 1);

                    break;
                }
            }
            
            // Probe next shift index of previous clue element index if current clue element index is reached to the end of shift index.
            if (shift_[0][cele_i_] > line_clue_->getMargin()) {
                --cele_i_;

                // Set previous shift index cele_cache to true if any solutions are found in current clue element index.
                if (getBitArrayElement(cele_cache_, cele_i_ + 1, 1) != 0b0) {
                    orBitArrayElement(cele_cache_, cele_i_, 0b1, 1);

                    // Set previous shift index to cache and fill the line of current clue element index if previous shift index is not set to cache.
                    // The order of setting a shift index to cache for each clue element index is guaranteed to be in ascending order.
                    if (getBitArrayElement(shift_cache_[true][cele_i_], shift_[0][cele_i_], 1) == 0b0) {
                        if (shift_[1][cele_i_] == 0) {
                            fill(false, line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_ - 1], line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_]);
                            fill(true, line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_], line_clue_->getOffset(cele_i_) + shift_[0][cele_i_] - 1);
                        } else {
                            fill(false, std::max(line_clue_->getOffset(cele_i_ - 1) + shift_[1][cele_i_], line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_ - 1]), line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_]);
                            fill(true, std::max(line_clue_->getOffset(cele_i_) + shift_[1][cele_i_] - 1, line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_]), line_clue_->getOffset(cele_i_) + shift_[0][cele_i_] - 1);
                        }
                        fill(false, line_clue_->getOffset(cele_i_) + shift_[0][cele_i_] - 1, line_clue_->getOffset(cele_i_) + shift_[0][cele_i_]);
                        shift_[1][cele_i_] = shift_[0][cele_i_];
                        orBitArrayElement(shift_cache_[true][cele_i_], shift_[0][cele_i_], 0b1, 1);
                    }
                } else {
                    orBitArrayElement(shift_cache_[false][cele_i_], shift_[0][cele_i_], 0b1, 1);
                }

                // Probe next shift index of previous clue element index.
                ++shift_[0][cele_i_];
                
                continue;
            }
        }

        // Check if any solutions are found.
        if (cele_cache_[0] & 1) {
            // Fill the back of the smallest shift index of the last clue element index with 0b01.
            for (ng_size_t shift_i{0};; ++shift_i) {
                if (getBitArrayElement(shift_cache_[true][line_clue_->getSize()], shift_i, 1) != 0b0) {
                    fill(false, line_clue_->getOffset(line_clue_->getSize()) + shift_i, line_clue_->getLineSize());
                    sweep();

                    break;
                }
            }

            return true;
        }
        return false;
    }
    bool LineSolver::infer(void) {
        for (; cele_i_ > 0;) {
            for (ng_size_t &shift_i{shift_[0][cele_i_]}; shift_i <= line_clue_->getMargin(); ++shift_i) {
                // Proceed to next shift index if current one is impossible.
                if (getBitArrayElement(shift_cache_[false][cele_i_], shift_i, 1) != 0b0) {
                    continue;
                }
                
                // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
                if (color_[1][line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_ - 1]] != color_[1][line_clue_->getOffset(cele_i_ - 1) + shift_i]) {
                    shift_i = line_clue_->getMargin() + 1;

                    break;
                }

                // Probe next clue element index if shift index of current one is not probed and not impossible.
                if (color_[0][line_clue_->getOffset(cele_i_ - 1) + shift_i] == color_[0][line_clue_->getOffset(cele_i_) + shift_i - 1]) {
                    ++cele_i_;
                    shift_[0][cele_i_] = shift_[0][cele_i_ - 1];
                    andBitArrayElement(cele_cache_, cele_i_, 0b0, 1);

                    break;
                }
            }

            // Probe next shift index of previous clue element index if current clue element index is reached to the end of shift index.
            if (shift_[0][cele_i_] > line_clue_->getMargin()) {
                --cele_i_;

                // Set previous shift index cele_cache to true if any solutions are found in current clue element index.
                if (getBitArrayElement(cele_cache_, cele_i_ + 1, 1) != 0b0) {
                    orBitArrayElement(cele_cache_, cele_i_, 0b1, 1);
                } else {
                    andBitArrayElement(shift_cache_[false][cele_i_], shift_[0][cele_i_], 0b0, 1);
                }

                // Probe next shift index of previous clue element index.
                ++shift_[0][cele_i_];
                
                continue;
            }
            
            if (cele_i_ == line_clue_->getSize() + 1) {
                if (getBitArrayElement(shift_cache_[true][cele_i_], shift_[0][cele_i_ - 1], 1) != 0b0) {
                    // Fill sweep_line and line.
                    std::memset(sweep_line_, 0, sizeof sweep_line_);
                    for (cele_i_ = 1; cele_i_ <= line_clue_->getSize(); ++cele_i_) {
                        fill(false, line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_ - 1], line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_]);
                        fill(true, line_clue_->getOffset(cele_i_ - 1) + shift_[0][cele_i_], line_clue_->getOffset(cele_i_) + shift_[0][cele_i_] - 1);
                        fill(false, line_clue_->getOffset(cele_i_) + shift_[0][cele_i_] - 1, line_clue_->getOffset(cele_i_) + shift_[0][cele_i_]);
                    }
                    fill(false, line_clue_->getOffset(line_clue_->getSize()) + shift_[0][line_clue_->getSize()], line_clue_->getLineSize());
                    sweep();

                    // Check current inferred shift index of the last clue element index is valid. 
                    orBitArrayElement(cele_cache_, cele_i_ - 1, 0b1, 1);

                    // Probe next shift index of previous clue element index.
                    cele_i_ = --cele_i_;
                    ++shift_[0][cele_i_];

                    // Try to solve inferred table if clue element index reached at the last and found a solution.
                    return true;
                }
            }
        }
        
        return false;
    }

    void LineSolver::set(const ng_size_t i, const ng_size_t val) {
        andBitArrayElement(line_, i, val, 2);
    }
    void LineSolver::fill(const bool color, const ng_size_t start, const ng_size_t end) {
        ++sweep_line_[color][start];
        --sweep_line_[color][end];
    }
    void LineSolver::sweep(void) {
        std::memset(line_, -1, sizeof line_);
        for (ng_size_t c{0}; c < 2; ++c) {
            for (ng_size_t vec_i{0}; vec_i < line_clue_->getLineSize(); ++vec_i) {
                if (sweep_line_[c][vec_i]) {
                    set(vec_i, 0b1 << c);
                }
                sweep_line_[c][vec_i + 1] += sweep_line_[c][vec_i];
            }
        }
    }
};
