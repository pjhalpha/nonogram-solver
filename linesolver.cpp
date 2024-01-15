#include "nonogram.hpp"

namespace Nonogram {
    LineSolver::LineSolver(Solver &solver, LineSolver *llink) : solver{solver}, clue{solver.clue}, llink{llink}, rlink{} {}

    void LineSolver::init(const ng_size_t vec, const ng_size_t vi) {
        // Initialize color.
        for (ng_size_t i{0}; i < clue.size[!vec]; ++i) {
            color[0][i + 1] = color[0][i] + (solver.get(vec, vi, i) == 0b01);
            color[1][i + 1] = color[1][i] + (solver.get(vec, vi, i) == 0b10);
        }
        color[0][clue.size[!vec] + 1] = color[0][clue.size[!vec]];
        color[1][clue.size[!vec] + 1] = color[1][clue.size[!vec]];
        // Initialize shift.
        shift[0][0] = shift[0][1] = 0;
        memset(shift[1], 0, sizeof shift[1]);
        // Initialize cache and subcache.
        memset(cache[true][0], -1, sizeof cache[true][0]);
        memset(cache[true][clue.clue_size[vec][vi] + 1], 0, sizeof cache[true][clue.clue_size[vec][vi] + 1]);
        for (ng_size_t ci{1}; ci <= clue.clue_size[vec][vi]; ++ci) {
            memset(cache[true][ci], 0, sizeof cache[true][ci]);
            memset(cache[false][ci], 0, sizeof cache[false][ci]);
            for (ng_size_t si{0}; si <= clue.margin[vec][vi]; ++si) {
                if (color[0][clue.offset[vec][vi][ci - 1] + si] != color[0][clue.offset[vec][vi][ci] + si - 1] || color[1][clue.offset[vec][vi][ci] + si - 1] != color[1][clue.offset[vec][vi][ci] + si]) {
                    orBitArrayElement(cache[false][ci], si, 0b1, 1);
                }
            }
        }
        for (ng_size_t si{0}; si <= clue.margin[vec][vi]; ++si) {
            if (color[1][clue.offset[vec][vi][clue.clue_size[vec][vi]] + si - 1] == color[1][clue.size[!vec]]) {
                for (; si <= clue.margin[vec][vi]; ++si) {
                    orBitArrayElement(cache[true][clue.clue_size[vec][vi] + 1], si, 0b1, 1);
                }
            } else {
                orBitArrayElement(cache[false][clue.clue_size[vec][vi]], si, 0b1, 1);
            }
        }
        memset(subcache, 0, sizeof subcache);
    }
    bool LineSolver::solve(const ng_size_t vec, const ng_size_t vi) {
        init(vec, vi);
        // Initialize line.
        memset(data, -1, sizeof data);
        for (ng_size_t ci{1}; ci > 0;) {
            for (ng_size_t &si{shift[0][ci]}; si <= clue.margin[vec][vi]; ++si) {
                // Proceed to next shift index if current one is impossible.
                if (getBitArrayElement(cache[false][ci], si, 1)) {
                    continue;
                }

                // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
                if (color[1][clue.offset[vec][vi][ci - 1] + shift[0][ci - 1]] != color[1][clue.offset[vec][vi][ci - 1] + si]) {
                    si = clue.margin[vec][vi] + 1;

                    break;
                }
                
                // Ignore current clue index if current shift index is known as a solution.
                if (getBitArrayElement(cache[true][ci + 1], shift[0][ci], 1)) {
                    ++ci;
                    shift[0][ci] = clue.margin[vec][vi] + 1;
                    orBitArrayElement(subcache, ci, 0b1, 1);

                    break;
                }

                // Probe next clue index if shift index of current one is not probed and not impossible.
                if (color[0][clue.offset[vec][vi][ci - 1] + si] == color[0][clue.offset[vec][vi][ci] + si - 1]) {
                    ++ci;
                    shift[0][ci] = shift[0][ci - 1];
                    andBitArrayElement(subcache, ci, 0b0, 1);

                    break;
                }
            }
            
            // Probe next shift index of previous clue index if current clue index is reached to the end of shift index.
            if (shift[0][ci] > clue.margin[vec][vi]) {
                --ci;
                // Set previous shift index subcache to true if any solutions are found in current clue index.
                if (getBitArrayElement(subcache, ci + 1, 1)) {
                    orBitArrayElement(subcache, ci, 0b1, 1);
                    // Set previous shift index to cache and fill the line of current clue index if previous shift index is not set to cache.
                    // The order of setting a shift index to cache for each clue index is guaranteed to be in ascending order.
                    if (!getBitArrayElement(cache[true][ci], shift[0][ci], 1)) {
                        if (!shift[1][ci]) {
                            fill(clue.offset[vec][vi][ci - 1] + shift[0][ci - 1], clue.offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                            fill(clue.offset[vec][vi][ci - 1] + shift[0][ci], clue.offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                        } else {
                            fill(std::max(clue.offset[vec][vi][ci - 1] + shift[1][ci], clue.offset[vec][vi][ci - 1] + shift[0][ci - 1]), clue.offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                            fill(std::max(clue.offset[vec][vi][ci] + shift[1][ci] - 1, clue.offset[vec][vi][ci - 1] + shift[0][ci]), clue.offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                        }
                        fill(clue.offset[vec][vi][ci] + shift[0][ci] - 1, clue.offset[vec][vi][ci] + shift[0][ci], 0b01);
                        shift[1][ci] = shift[0][ci];
                        orBitArrayElement(cache[true][ci], shift[0][ci], 0b1, 1);
                    }
                } else {
                    orBitArrayElement(cache[false][ci], shift[0][ci], 0b1, 1);
                }
                // Probe next shift index of previous clue index.
                ++shift[0][ci];
                
                continue;
            }
        }

        // Check if any solutions are found.
        if (subcache[0] & 1) {
            // Fill in the back of the smallest shift index of the last clue index with 0b01.
            for (ng_size_t si{0};; ++si) {
                if (getBitArrayElement(cache[true][clue.clue_size[vec][vi]], si, 1)) {
                    fill(clue.offset[vec][vi][clue.clue_size[vec][vi]] + si, clue.size[!vec], 0b01);

                    break;
                }
            }

            return true;
        }
        return false;
    }
    bool LineSolver::infer(const ng_size_t vec, const ng_size_t vi) {
        init(vec, vi);
        for (ng_size_t ci{1}; ci > 0;) {
            for (ng_size_t &si{shift[0][ci]}; si <= clue.margin[vec][vi]; ++si) {
                // Proceed to next shift index if current one is impossible.
                if (getBitArrayElement(cache[false][ci], si, 1)) {
                    continue;
                }
                
                // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
                if (color[1][clue.offset[vec][vi][ci - 1] + shift[0][ci - 1]] != color[1][clue.offset[vec][vi][ci - 1] + si]) {
                    si = clue.margin[vec][vi] + 1;

                    break;
                }

                // Probe next clue index if shift index of current one is not probed and not impossible.
                if (color[0][clue.offset[vec][vi][ci - 1] + si] == color[0][clue.offset[vec][vi][ci] + si - 1]) {
                    ++ci;
                    shift[0][ci] = shift[0][ci - 1];
                    andBitArrayElement(subcache, ci, 0b0, 1);

                    break;
                }
            }

            // Probe next shift index of previous clue index if current clue index is reached to the end of shift index.
            if (shift[0][ci] > clue.margin[vec][vi]) {
                --ci;
                // Set previous shift index subcache to true if any solutions are found in current clue index.
                if (getBitArrayElement(subcache, ci + 1, 1)) {
                    orBitArrayElement(subcache, ci, 0b1, 1);
                } else {
                    andBitArrayElement(cache[false][ci], shift[0][ci], 0b0, 1);
                }
                // Probe next shift index of previous clue index.
                ++shift[0][ci];
                
                continue;
            }
            
            // Fill in the line and try to solve inferred table if clue index reached at the last and found a solution.
            if (ci == clue.clue_size[vec][vi] + 1) {
                if (getBitArrayElement(cache[true][ci], shift[0][ci - 1], 1)) {
                    memset(data, -1, sizeof data);
                    for (ci = 1; ci <= clue.clue_size[vec][vi]; ++ci) {
                        fill(clue.offset[vec][vi][ci - 1] + shift[0][ci - 1], clue.offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                        fill(clue.offset[vec][vi][ci - 1] + shift[0][ci], clue.offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                        fill(clue.offset[vec][vi][ci] + shift[0][ci] - 1, clue.offset[vec][vi][ci] + shift[0][ci], 0b01);
                    }
                    fill(clue.offset[vec][vi][clue.clue_size[vec][vi]] + shift[0][clue.clue_size[vec][vi]], clue.size[!vec], 0b01);

                    if (solver.next(vec, vi)) {
                        return true;
                    }

                    orBitArrayElement(subcache, ci - 1, 0b1, 1);
                }

                // Probe next shift index of previous clue index.
                --ci;
                ++shift[0][ci];
            }
        }
        
        return false;
    }
    ng_sq_size_t LineSolver::get(const ng_size_t i) const {
        return getBitArrayElement(data, i, 2);
    }
    void LineSolver::set(const ng_size_t i, const ng_size_t v) {
        andBitArrayElement(data, i, v, 2);
    }
    void LineSolver::fill(const ng_size_t s, const ng_size_t e, const ng_size_t v) {
        for (ng_size_t i{s}; i < e; ++i) {
            set(i, v);
        }
    }
};
