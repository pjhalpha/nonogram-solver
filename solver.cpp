#include "nonogram.hpp"

namespace Nonogram {
    Solver::Solver(void) : line{new LineSolver{*this, nullptr}}, wrap{}, inspect{} {}
    Solver::~Solver(void) {
        delete line;
    }

    void Solver::setWrap(bool (*wrap_ptr)(void)) {
        wrap = wrap_ptr;
    }
    void Solver::setInspect(void (*inspect_ptr)(const ng_size_t, const ng_size_t, const bool)) {
        inspect = inspect_ptr;
    }
    bool Solver::solve(const ng_size_t* const input_arr) {
        // Initialize depth, count and time_start and solve.
        depth = 0;
        count = 0;
        time_start = clock();
        if (clue.input(input_arr) && init()) solve();

        // Free every line solvers except the first one.
        for (LineSolver *prev{line->rlink}, *cur; prev; prev = cur) {
            cur = prev->rlink;
            delete prev;
        }

        return count;
    }
    ng_size_t Solver::get(const ng_size_t r, const ng_size_t c) const {
        return depth_table[r][c] <= depth ? getBitArrayElement(table[r], c, 2) : 0b00;
    }
    ng_size_t Solver::get(const bool vec, const ng_size_t vi, const ng_size_t i) const {
        return get(vec ? i : vi, vec ? vi : i);
    }
    ng_sq_size_t Solver::getRemain(void) const {
        return remain;
    }    
    ng_size_t Solver::getCount(void) const {
        return count;
    }
    long double Solver::getTime(void) const {
        return time_start == -1 || time_end == -1 ? -1 : static_cast<long double>(time_end - time_start) / CLOCKS_PER_SEC;
    }

    bool Solver::init(void) {
        // Initialize table, dpeth_table, remain, state, line_remain and line_state.
        memset(table, 0, sizeof table);
        memset(depth_table, 0, sizeof depth_table);
        remain = clue.size[0] * clue.size[1];
        state = clue.size[0] + clue.size[1];
        for (ng_size_t vec{0}; vec < 2; ++vec) for (ng_size_t vi{0}; vi < clue.size[vec]; ++vi) {
            line_remain[vec][vi] = clue.size[!vec];
        }
        memset(line_state, -1, sizeof line_state);

        // Fill lines whose clue is 0 and is not 0.
        for (ng_size_t vec{0}; vec < 2; ++vec) for (ng_size_t vi{0}; vi < clue.size[vec]; ++vi) {
            if (clue.offset[vec][vi][1] == 1) {
                fill(vec, vi, 0, clue.size[!vec], 0b01);
            } else {
                for (ng_size_t ci{0}; ci < clue.clue_size[vec][vi]; ++ci) {
                    fill(vec, vi, clue.offset[vec][vi][ci] + clue.margin[vec][vi], clue.offset[vec][vi][ci + 1] - 1, 0b10);
                }
            }
            if (inspect) inspect(vec, vi, false);
        }

        // Set state to false if all pixels of a line are gray.
        for (ng_size_t vec{0}; vec < 2; ++vec) for (ng_size_t vi{0}; vi < clue.size[vec]; ++vi) {
            if (!line_remain[vec][vi] || line_remain[vec][vi] == clue.size[!vec]) {
                --state;
                andBitArrayElement(line_state[vec], vi, 0b0, 1);
            }
        }

        return true;
    }
    bool Solver::solve(void) {
        while (true) {
            // Solve a line that can be updated.
            for (ng_size_t vec{0}; vec < 2; ++vec) for (ng_size_t vi{0}; vi < clue.size[vec]; ++vi) {
                if (getBitArrayElement(line_state[vec], vi, 1)) {
                    if (line->solve(vec, vi)) {
                        merge(vec, vi);
                        if (inspect) inspect(vec, vi, false);
                    } else {
                        return false;
                    }
                }
            }

            // Verify or infer the table if no line can be updated.
            if (!state) {
                if (!remain) {
                    if (verify()) {
                        ++count;
                        time_end = clock();
                        
                        return wrap ? !wrap() : false;
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
        for (ng_size_t vi{0}; vi < clue.size[1]; ++vi) {
            if (line_remain[1][vi]) {
                return line->infer(1, vi);
            }
        }
        return false;
    }
    void Solver::merge(const ng_size_t vec, const ng_size_t vi) {
        ng_sq_size_t changed{0};

        // Merge the table with the solved line and update table, remain, state, line_remain and line_state.
        for (ng_size_t i{0}; i < clue.size[!vec]; ++i) {
            if (!get(vec, vi, i) && line->get(i)) {
                set(vec, vi, i, line->get(i));
                ++changed;
                --line_remain[!vec][i];
                if (!getBitArrayElement(line_state[!vec], i, 1)) {
                    ++state;
                    orBitArrayElement(line_state[!vec], i, 0b1, 1);
                }
            }
        }
        remain -= changed;
        --state;
        line_remain[vec][vi] -= changed;
        andBitArrayElement(line_state[vec], vi, 0b0, 1);
    }
    bool Solver::next(const ng_size_t vec, const ng_size_t vi) {
        // Merge the table with the infered line and update depth and state.
        ++depth;
        ++state;
        merge(vec, vi);

        // Create new link if there are no next link.
        if (!line->rlink) {
            line->rlink = new LineSolver{*this, line};
        }
        line = line->rlink;

        if (inspect) inspect(vec, vi, true);

        if (solve()) {
            return true;
        }

        // Initialize to before the inference.
        line = line->llink;
        for (int r{0}; r < clue.size[0]; ++r) for (int c{0}; c < clue.size[1]; ++c) {
            if (depth_table[r][c] == depth) {
                andBitArrayElement(table[r], c, 0b00, 2);
                ++remain;
                ++line_remain[0][r];
                ++line_remain[1][c];
                depth_table[r][c] = 0;
            }
        }
        state = 0;
        memset(line_state, 0, sizeof line_state);
        --depth;

        return false;
    }
    bool Solver::verify(void) {
        // Solve the table again to verify.
        for (ng_size_t vec{0}; vec < 2; ++vec) for (ng_size_t vi{0}; vi < clue.size[vec]; ++vi) {
            if (clue.offset[vec][vi][1] != 1 && !line->solve(vec, vi)) {
                return false;
            }
        }
        return true;
    }
    void Solver::set(const ng_size_t r, const ng_size_t c, const ng_size_t v) {
        orBitArrayElement(table[r], c, v, 2);
        depth_table[r][c] = depth;
    }
    void Solver::set(const bool vec, const ng_size_t vi, const ng_size_t i, const ng_size_t v) {
        set(vec ? i : vi, vec ? vi : i, v);
    }
    void Solver::fill(const bool vec, const ng_size_t vi, const ng_size_t s, const ng_size_t e, const ng_size_t v) {
        // Set a pixel and update remain if a pixel is not a gray.
        for (ng_size_t i{s}; i < e; ++i) {
            if (!get(vec, vi, i)) {
                set(vec, vi, i, v);
                --remain;
                --line_remain[0][vec ? i : vi];
                --line_remain[1][vec ? vi : i];
            }
        }
    }
};
