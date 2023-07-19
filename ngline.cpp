#include "nonogram.hpp"

const uint16_t (&NonogramLine::size)[2]{Nonogram::size};
const uint16_t (&NonogramLine::offset)[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]{Nonogram::offset};
const uint16_t (&NonogramLine::clue_size)[2][MAX_NG_SIZE]{Nonogram::clue_size};
const uint16_t (&NonogramLine::margin)[2][MAX_NG_SIZE]{Nonogram::margin};

NonogramLine::NonogramLine(NonogramTable* const table) : table{table} {
}

void NonogramLine::init(const uint16_t vec, const uint16_t vi) {
    // Initialize base.
    std::fill(base, base + size[!vec], 0b11);
    // Initialize color.
    for (uint16_t i{0}; i < size[!vec]; i++) {
        color[0][i + 1] = color[0][i] + (table->get(vec, vi, i) == 0b01);
        color[1][i + 1] = color[1][i] + (table->get(vec, vi, i) == 0b10);
    }
    color[0][size[!vec] + 1] = color[0][size[!vec]];
    color[1][size[!vec] + 1] = color[1][size[!vec]];
    // Initialize shift.
    shift[0][0] = shift[0][1] = 0;
    std::fill(&shift[1][0], &shift[1][clue_size[vec][vi] + 2], 0);
    // Initialize cache and subcache.
    std::fill(&cache[true][0][0], &cache[true][0][size[!vec]], true);
    std::fill(&cache[true][clue_size[vec][vi] + 1][0], &cache[true][clue_size[vec][vi] + 1][size[!vec]], false);
    for (uint16_t ci{1}; ci <= clue_size[vec][vi]; ci++) {
        std::fill(&cache[true][ci][0], &cache[true][ci][size[!vec]], false);
        std::fill(&cache[false][ci][0], &cache[false][ci][size[!vec]], false);
        for (uint16_t si{0}; si <= margin[vec][vi]; si++) {
            if (color[0][offset[vec][vi][ci - 1] + si] != color[0][offset[vec][vi][ci] + si - 1] || color[1][offset[vec][vi][ci] + si - 1] != color[1][offset[vec][vi][ci] + si]) {
                cache[false][ci][si] = true;
            }
        }
    }
    for (uint16_t si{0}; si <= margin[vec][vi]; si++) {
        if (color[1][offset[vec][vi][clue_size[vec][vi]] + si - 1] == color[1][size[!vec]]) {
            for (; si <= margin[vec][vi]; si++) {
                cache[true][clue_size[vec][vi] + 1][si] = true;
            }
        } else {
            cache[false][clue_size[vec][vi]][si] = true;
        }
    }
    std::fill(subcache, subcache + clue_size[vec][vi] + 2, false);
}
void NonogramLine::fill(const uint16_t begin, const uint16_t end, const uint16_t v) {
    for (uint16_t i{begin}; i < end; i++) {
        base[i] &= v;
    }
}
bool NonogramLine::solve(const uint16_t vec, const uint16_t vi) {
    init(vec, vi);
    for (uint16_t ci{1}; ci > 0;) {
        for (uint16_t &si{shift[0][ci]}; si <= margin[vec][vi]; si++) {
            // Proceed to next shift index if current one is impossible.
            if (cache[false][ci][si]) {
                continue;
            }

            // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
            if (color[1][offset[vec][vi][ci - 1] + shift[0][ci - 1]] != color[1][offset[vec][vi][ci - 1] + si]) {
                si = margin[vec][vi] + 1;

                break;
            }
            
            // Ignore current clue index if current shift index is known as a solution.
            if (cache[true][ci + 1][shift[0][ci]]) {
                ci++;
                shift[0][ci] = margin[vec][vi] + 1;
                subcache[ci] = true;

                break;
            }

            // Probe next clue index if shift index of current one is not probed and not impossible.
            if (color[0][offset[vec][vi][ci - 1] + si] == color[0][offset[vec][vi][ci] + si - 1]) {
                ci++;
                shift[0][ci] = shift[0][ci - 1];
                subcache[ci] = false;

                break;
            }
        }
        
        // Probe next shift index of previous clue index if current clue index is reached to the end of shift index.
        if (shift[0][ci] > margin[vec][vi]) {
            ci--;
            // Set previous shift index subcache to true if any solutions are found in current clue index.
            if (subcache[ci + 1]) {
                subcache[ci] = true;
                // Set previous shift index to cache and fill the line of current clue index if previous shift index is not set to cache (the order of setting a shift index to cache for each clue index is guaranteed to be in ascending order).
                if (!cache[true][ci][shift[0][ci]]) {
                    if (shift[1][ci] == 0) {
                        fill(offset[vec][vi][ci - 1] + shift[0][ci - 1], offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                        fill(offset[vec][vi][ci - 1] + shift[0][ci], offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                    } else {
                        fill(std::max(offset[vec][vi][ci - 1] + shift[1][ci], offset[vec][vi][ci - 1] + shift[0][ci - 1]), offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                        fill(std::max(offset[vec][vi][ci] + shift[1][ci] - 1, offset[vec][vi][ci - 1] + shift[0][ci]), offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                    }
                    base[offset[vec][vi][ci] + shift[0][ci] - 1] &= 0b01;
                    shift[1][ci] = shift[0][ci];
                    cache[true][ci][shift[0][ci]] = true;
                }
            } else {
                cache[false][ci][shift[0][ci]] = true;
            }
            // Probe next shift index of previous clue index.
            shift[0][ci]++;
            
            continue;
        }
    }

    // Check if any solutions are found.
    if (subcache[0]) {
        // Fill in the back of the smallest shift index of the last clue index with 0b01.
        for (uint16_t si{0};; si++) {
            if (cache[true][clue_size[vec][vi]][si]) {
                fill(offset[vec][vi][clue_size[vec][vi]] + si, size[!vec], 0b01);

                break;
            }
        }

        return true;
    }
    return false;
}
bool NonogramLine::infer(const Nonogram* const ng, const uint16_t vec, const uint16_t vi) {
    init(vec, vi);
    for (uint16_t ci{1}; ci > 0;) {
        for (uint16_t &si{shift[0][ci]}; si <= margin[vec][vi]; si++) {
            // Proceed to next shift index if current one is impossible.
            if (cache[false][ci][si]) {
                continue;
            }
            
            // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
            if (color[1][offset[vec][vi][ci - 1] + shift[0][ci - 1]] != color[1][offset[vec][vi][ci - 1] + si]) {
                si = margin[vec][vi] + 1;

                break;
            }

            // Probe next clue index if shift index of current one is not probed and not impossible.
            if (color[0][offset[vec][vi][ci - 1] + si] == color[0][offset[vec][vi][ci] + si - 1]) {
                ci++;
                shift[0][ci] = shift[0][ci - 1];
                subcache[ci] = false;

                break;
            }
        }

        // Probe next shift index of previous clue index if current clue index is reached to the end of shift index.
        if (shift[0][ci] > margin[vec][vi]) {
            ci--;
            // Set previous shift index subcache to true if any solutions are found in current clue index.
            if (subcache[ci + 1]) {
                subcache[ci] = true;
            } else {
                cache[false][ci][shift[0][ci]] = true;
            }
            // Probe next shift index of previous clue index.
            shift[0][ci]++;
            
            continue;
        }
        
        // Fill in the line and try to solve inferred table if clue index reached at the last and found a solution.
        if (ci == clue_size[vec][vi] + 1) {
            if (cache[true][ci][shift[0][ci - 1]]) {
                for (ci = 1; ci <= clue_size[vec][vi]; ci++) {
                    std::fill(&base[offset[vec][vi][ci - 1] + shift[0][ci - 1]], &base[offset[vec][vi][ci - 1] + shift[0][ci]], 0b01);
                    std::fill(&base[offset[vec][vi][ci - 1] + shift[0][ci]], &base[offset[vec][vi][ci] + shift[0][ci] - 1], 0b10);
                    base[offset[vec][vi][ci] + shift[0][ci] - 1] = 0b01;
                }
                std::fill(&base[offset[vec][vi][clue_size[vec][vi]] + shift[0][clue_size[vec][vi]]], &base[size[!vec] + 1], 0b01);
                
                if (Nonogram{ng, vec, vi}.solve()) {
                    return true;
                }

                subcache[ci - 1] = true;
            }

            // Probe next shift index of previous clue index.
            ci--;
            shift[0][ci]++;
        }
    }
    
    return false;
}
