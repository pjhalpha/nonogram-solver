#include "nonogram.hpp"

const uint16_t (&NonogramLine::size)[2]{Nonogram::size};
const uint16_t (&NonogramLine::offset)[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]{Nonogram::offset};
const uint16_t (&NonogramLine::clue_size)[2][MAX_NG_SIZE]{Nonogram::clue_size};
const uint16_t (&NonogramLine::margin)[2][MAX_NG_SIZE]{Nonogram::margin};

NonogramLine::NonogramLine(NonogramTable* const table) : table{table} {
}

void NonogramLine::init(const uint16_t vec, const uint16_t vi) {
    base.set();
    temp.set();
    for (uint16_t ci{0}; ci < size[!vec]; ci++) {
        if (table->get(vec, vi, ci) != 0b00) {
            setBinBit(base, ci, table->get(vec, vi, ci));
        }
        black[ci + 1] = black[ci] + (table->get(vec, vi, ci) == 0b10);
        white[ci + 1] = white[ci] + (table->get(vec, vi, ci) == 0b01);
    }
    possible[0].set();
    possible[clue_size[vec][vi] + 1].reset();
    shift[0] = shift[1] = 0;
    find.reset();
    // Update possible and impossible.
    for (uint16_t ci{1}; ci <= clue_size[vec][vi]; ci++) {
        possible[ci].reset();
        impossible[ci].reset();
        for (uint16_t si{0}; si <= margin[vec][vi]; si++) {
            if (white[offset[vec][vi][ci - 1] + si] != white[offset[vec][vi][ci] + si - 1]) {
                impossible[ci].set(si);
            }
        }
    }
    for (uint16_t si{0}; si <= margin[vec][vi]; si++) {
        if (black[offset[vec][vi][clue_size[vec][vi]] + si - 1] == black[size[!vec]]) {
            for (; si <= margin[vec][vi]; si++) {
                possible[clue_size[vec][vi] + 1].set(si);
            }
        } else {
            impossible[clue_size[vec][vi]].set(si);
        }
    }
}
bool NonogramLine::solve(const uint16_t vec, const uint16_t vi) {
    init(vec, vi);
    for (uint16_t ci{1}; ci > 0;) {
        for (uint16_t &si{shift[ci]}; si <= margin[vec][vi]; si++) {
            // Proceed to next shift index if current one is impossible.
            if (impossible[ci].test(si)) {
                continue;
            }

            // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
            if (black[std::max(offset[vec][vi][ci - 1] + shift[ci - 1] - 1, 0)] != black[offset[vec][vi][ci - 1] + si]) {
                si = margin[vec][vi] + 1;

                break;
            }
            
            // Ignore current clue index if current shift index is known as a solution.
            if (possible[ci + 1].test(shift[ci])) {
                ci++;
                find.set(ci);
                shift[ci] = margin[vec][vi] + 1;

                break;
            }

            // Probe next clue index if shift index of current one is not probed and not impossible.
            if (white[offset[vec][vi][ci - 1] + si] == white[offset[vec][vi][ci] + si - 1]) {
                ci++;
                find.reset(ci);
                shift[ci] = shift[ci - 1];

                break;
            }
        }
        
        // Probe next shift index of previous clue index if current clue index is reached to the end of shift index.
        if (shift[ci] > margin[vec][vi]) {
            ci--;
            // Set previous shift index to found if any solutions are found in current clue index.
            if (find.test(ci + 1)) {
                find.set(ci);
                // Set previous shift index to possible and fill the line of current clue index if previous shift index is not set to possible.
                if (!possible[ci].test(shift[ci])) {
                    possible[ci].set(shift[ci]);
                    fillBinBit(temp, std::max(offset[vec][vi][ci - 1] + shift[ci - 1] - 1, 0), offset[vec][vi][ci - 1] + shift[ci], 0b01);
                    fillBinBit(temp, offset[vec][vi][ci - 1] + shift[ci], offset[vec][vi][ci] + shift[ci] - 1, 0b10);
                    base &= temp;
                }
            } else {
                impossible[ci].set(shift[ci]);
            }
            // Probe next shift index of previous clue index.
            shift[ci]++;
            
            continue;
        }
    }
    
    // Check if any solutions are found.
    if (find.test(0)) {
        for (uint16_t si{0};; si++) {
            // Fill in the back of the smallest shift index of the last clue index with 0b01.
            if (possible[clue_size[vec][vi]].test(si)) {
                fillBinBit(temp, offset[vec][vi][clue_size[vec][vi]] + si - 1, size[!vec], 0b01);
                base &= temp;

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
        // Fill in the line and try to solve inferred table if clue index reached at the last and found a solution.
        if (ci == clue_size[vec][vi] + 1) {
            if (possible[ci].test(shift[ci - 1])) {
                for (ci = 1; ci <= clue_size[vec][vi]; ci++) {
                    fillBinBit(base, std::max(offset[vec][vi][ci - 1] + shift[ci - 1] - 1, 0), offset[vec][vi][ci - 1] + shift[ci], 0b01);
                    fillBinBit(base, offset[vec][vi][ci - 1] + shift[ci], offset[vec][vi][ci] + shift[ci] - 1, 0b10);
                }
                fillBinBit(base, offset[vec][vi][clue_size[vec][vi]] + shift[clue_size[vec][vi]] - 1, size[!vec], 0b01);

                if (Nonogram{ng, vec, vi}.solve()) {
                    return true;
                }

                find.set(ci - 1);
            }

            // Probe next shift index of previous clue index.
            ci--;
            shift[ci]++;
        }

        for (uint16_t &si{shift[ci]}; si <= margin[vec][vi]; si++) {
            // Proceed to next shift index if current one is impossible.
            if (impossible[ci].test(si)) {
                continue;
            }
            
            // Ignore the left shift index if the case setting 0b10 to 0b01 is found.
            if (black[std::max(offset[vec][vi][ci - 1] + shift[ci - 1] - 1, 0)] != black[offset[vec][vi][ci - 1] + si]) {
                si = margin[vec][vi] + 1;

                break;
            }

            // Probe next clue index if shift index of current one is not probed and not impossible.
            if (white[offset[vec][vi][ci - 1] + si] == white[offset[vec][vi][ci] + si - 1]) {
                ci++;
                find.reset(ci);
                shift[ci] = shift[ci - 1];

                break;
            }
        }

        // Probe next shift index of previous clue index if current clue index is reached to the end of current shift index.
        if (shift[ci] > margin[vec][vi]) {
            ci--;
            // Set previous shift index to find if any solutions are found in current clue index.
            if (find.test(ci + 1)) {
                find.set(ci);
            } else {
                impossible[ci].set(shift[ci]);
            }
            // Probe next shift index of previous clue index.
            shift[ci]++;
            
            continue;
        }
    }
    
    return false;
}
