#include "solver.hpp"

namespace Nonogram {
    ng_size_t Solver::get(const ng_size_t row, const ng_size_t col) const {
        return (depth_table_[row][col] <= depth_) ? getBitArrayElement(table_[row], col, 2) : 0b00;
    }
    ng_size_t Solver::get(const bool vec, const ng_size_t vec_i, const ng_size_t cross_i) const {
        return get(vec ? cross_i : vec_i, vec ? vec_i : cross_i);
    }
    ng_sq_size_t Solver::getRemain(void) const {
        return remain_;
    }    
    ng_sq_size_t Solver::getCount(void) const {
        return count_;
    }
    double Solver::getTime(void) const {
        return (time_start_ == -1 || time_end_ == -1) ? -1 : static_cast<double>(time_end_ - time_start_) / CLOCKS_PER_SEC;
    }
    bool Solver::solve(const Clue &clue) {
        // Initialize clue_, line_solver_, depth_, count_ and time_start_ and solve the nonogram.
        clue_ = &clue;
        line_solver_ = new LineSolver{nullptr};
        depth_ = 0;
        count_ = 0;
        time_start_ = clock();
        if (init()) {
            solve();
        }

        // Free all line solvers.
        for (LineSolver *prev_link{line_solver_}, *cur_link; cur_link; prev_link = cur_link) {
            cur_link = prev_link->getNextLink();
            std::free(prev_link);
        }

        return count_;
    }
    bool Solver::inspect(const bool vec, const ng_size_t vec_i, const bool is_inference) {
        return true;
    }
    bool Solver::wrap(void) {
        return true;
    }

    const ng_size_t *Solver::getLine(const bool vec, const ng_size_t vec_i) {
        static ng_size_t line[MAX_SIZE];

        for (ng_size_t cross_i{0}; cross_i < clue_->getSize(!vec); ++cross_i) {
            line[cross_i] = get(vec, vec_i, cross_i);
        }

        return line;
    }
    void Solver::set(const ng_size_t row, const ng_size_t col, const ng_size_t val) {
        orBitArrayElement(table_[row], col, val, 2);
        depth_table_[row][col] = depth_;
    }
    void Solver::set(const bool vec, const ng_size_t vec_i, const ng_size_t cross_i, const ng_size_t val) {
        set(vec ? cross_i : vec_i, vec ? vec_i : cross_i, val);
    }
    void Solver::fill(const bool vec, const ng_size_t vec_i, const ng_size_t start, const ng_size_t end, const ng_size_t val) {
        // Set a pixel and update remain_ if a pixel is not a gray.
        for (ng_size_t cross_i{start}; cross_i < end; ++cross_i) {
            if (get(vec, vec_i, cross_i) == 0b00) {
                set(vec, vec_i, cross_i, val);
                --remain_;
                --line_remain_[0][vec ? cross_i : vec_i];
                --line_remain_[1][vec ? vec_i : cross_i];
            }
        }
    }
    bool Solver::init(void) {
        // Initialize table_, dpeth_table_, remain_, state_, line_remain_ and line_state_.
        std::memset(table_, 0, sizeof table_);
        std::memset(depth_table_, 0, sizeof depth_table_);
        remain_ = clue_->getSize(0) * clue_->getSize(1);
        state_ = clue_->getSize(0) + clue_->getSize(1);
        for (ng_size_t vec{0}; vec < 2; ++vec) {
            for (ng_size_t vec_i{0}; vec_i < clue_->getSize(vec); ++vec_i) {
                line_remain_[vec][vec_i] = clue_->getSize(!vec);
            }
        }
        std::memset(line_state_, -1, sizeof line_state_);

        // Fill lines whose clue is 0 and is not 0.
        for (ng_size_t vec{0}; vec < 2; ++vec) {
            for (ng_size_t vec_i{0}; vec_i < clue_->getSize(vec); ++vec_i) {
                if (clue_->getLineClue(vec, vec_i).getOffset(1) == 1) {
                    fill(vec, vec_i, 0, clue_->getSize(!vec), 0b01);
                } else {
                    for (ng_size_t cele_i{0}; cele_i < clue_->getLineClue(vec, vec_i).getSize(); ++cele_i) {
                        fill(vec, vec_i, clue_->getLineClue(vec, vec_i).getOffset(cele_i) + clue_->getLineClue(vec, vec_i).getMargin(), clue_->getLineClue(vec, vec_i).getOffset(cele_i + 1) - 1, 0b10);
                    }
                }

                if (!inspect(vec, vec_i, false)) {
                    return false;
                }
            }
        }

        // Set state_ to false if all pixels of a line are gray.
        for (ng_size_t vec{0}; vec < 2; ++vec) {
            for (ng_size_t vec_i{0}; vec_i < clue_->getSize(vec); ++vec_i) {
                if (line_remain_[vec][vec_i] == 0 || line_remain_[vec][vec_i] == clue_->getSize(!vec)) {
                    --state_;
                    andBitArrayElement(line_state_[vec], vec_i, 0b0, 1);
                }
            }
        }

        return true;
    }
    bool Solver::solve(void) {
        while (true) {
            // Solve a line that can be updated.
            for (ng_size_t vec{0}; vec < 2; ++vec) {
                for (ng_size_t vec_i{0}; vec_i < clue_->getSize(vec); ++vec_i) {
                    if (getBitArrayElement(line_state_[vec], vec_i, 1) != 0b0) {
                        line_solver_->init(clue_->getLineClue(vec, vec_i), getLine(vec, vec_i));
                        if (line_solver_->solve()) {
                            merge(vec, vec_i);
                            
                            if (!inspect(vec, vec_i, false)) {
                                return false;
                            }
                        } else {
                            return false;
                        }
                    }
                }
            }

            // Verify or infer the table if no line can be updated.
            if (state_ == 0) {
                if (remain_ == 0) {
                    if (verify()) {
                        ++count_;
                        time_end_ = clock();
                        
                        return !wrap();
                    }
                    return false;
                } else {
                    return infer();
                }
            }
        }
    }
    bool Solver::infer(void) {
        // Infer a column line that is not full.
        for (ng_size_t vec_i{0}; vec_i < clue_->getSize(1); ++vec_i) {
            if (line_remain_[1][vec_i]) {
                line_solver_->init(clue_->getLineClue(1, vec_i), getLine(1, vec_i));

                while (line_solver_->infer()) {
                    if (next(1, vec_i)) {
                        return true;
                    }
                }
                return false;
            }
        }
        return false;
    }
    void Solver::merge(const bool vec, const ng_size_t vec_i) {
        ng_sq_size_t changed{0};

        // Merge the table with the solved line and update table_, remain_, state_, line_remain_ and line_state_.
        for (ng_size_t cross_i{0}; cross_i < clue_->getSize(!vec); ++cross_i) {
            if (get(vec, vec_i, cross_i) == 0b00 && line_solver_->get(cross_i) != 0b00) {
                set(vec, vec_i, cross_i, line_solver_->get(cross_i));
                ++changed;
                --line_remain_[!vec][cross_i];
                if (getBitArrayElement(line_state_[!vec], cross_i, 1) == 0b0) {
                    ++state_;
                    orBitArrayElement(line_state_[!vec], cross_i, 0b1, 1);
                }
            }
        }
        remain_ -= changed;
        --state_;
        line_remain_[vec][vec_i] -= changed;
        andBitArrayElement(line_state_[vec], vec_i, 0b0, 1);
    }
    bool Solver::next(const bool vec, const ng_size_t vec_i) {
        // Merge the table with the infered line and update depth_ and state_.
        ++depth_;
        ++state_;
        merge(vec, vec_i);

        // Create new link if there are no next link.
        if (!line_solver_->getNextLink()) {
            line_solver_->setNextLink(new LineSolver{line_solver_});
        }
        line_solver_ = line_solver_->getNextLink();

        if (!inspect(vec, vec_i, true)) {
            return false;
        }

        if (solve()) {
            return true;
        }

        // Initialize to before the inference.
        line_solver_ = line_solver_->getPrevLink();
        for (int row{0}; row < clue_->getSize(0); ++row) {
            for (int col{0}; col < clue_->getSize(1); ++col) {
                if (depth_table_[row][col] == depth_) {
                    andBitArrayElement(table_[row], col, 0b00, 2);
                    ++remain_;
                    ++line_remain_[0][row];
                    ++line_remain_[1][col];
                    depth_table_[row][col] = 0;
                }
            }
        }
        state_ = 0;
        std::memset(line_state_, 0, sizeof line_state_);
        --depth_;

        return false;
    }
    bool Solver::verify(void) {
        // Solve the table again to verify.
        for (ng_size_t vec{0}; vec < 2; ++vec) {
            for (ng_size_t vec_i{0}; vec_i < clue_->getSize(vec); ++vec_i) {
                if (clue_->getLineClue(vec, vec_i).getOffset(1) != 1) {
                    line_solver_->init(clue_->getLineClue(vec, vec_i), getLine(vec, vec_i));
                    if (!line_solver_->solve()) {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
};
