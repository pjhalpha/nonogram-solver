#include "nonogram.hpp"

char printBinBit(const uint16_t v) {
    return v == 0b10 ? '#' : v == 0b01 ? '.' : v == 0b00 ? '?' : '!';
}

Nonogram::Nonogram(const Nonogram* const prev, const uint16_t vec, const uint16_t vi) {
    copy(prev);
    state++;
    memcpy(line, prev->line, sizeof line);
    merge(vec, vi);
}
Nonogram &Nonogram::getInstance(void) {
    static Nonogram instance;

    return instance;
}

bool Nonogram::input(std::istream &is) {
    std::string buf;
    std::stringstream ls;
    uint32_t clue_sum[2]{0};

    // Get the number of rows and columns of the table.
    std::getline(is, buf);
    ls = std::stringstream{buf};
    ls >> size[0] >> size[1];
    
    if (ls >> size[0] || size[0] > MAX_SIZE || size[1] > MAX_SIZE) return false;

    // Get the clue and initialize offset, clue_size and margin.
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        uint16_t &line_clue_size{clue_size[vec][vi]};

        std::getline(is, buf);
        ls = std::stringstream{buf};
        for (line_clue_size = 0; line_clue_size < MAX_CLUE_SIZE && ls >> offset[vec][vi][line_clue_size + 1]; line_clue_size++) {
            offset[vec][vi][line_clue_size + 1] += offset[vec][vi][line_clue_size] + 1;
        }

        if (!line_clue_size || ls >> offset[vec][vi][line_clue_size + 1] || line_clue_size != 1 && !offset[vec][vi][1]) return false;

        margin[vec][vi] = size[!vec] - offset[vec][vi][line_clue_size] + 1;
        clue_sum[vec] += offset[vec][vi][line_clue_size] - line_clue_size;
    
        if (margin[vec][vi] < 0) return false;
    }
    return clue_sum[0] == clue_sum[1];
}

void Nonogram::merge(const uint16_t vec, const uint16_t vi) {
    uint32_t changed{0};

    // Merge the table with the solved line and update table, remain, state, line_remain and line_state.
    for (uint16_t i{0}; i < size[!vec]; i++) {
        if (!get(vec, vi, i) && line[i >> 4] >> ((i & 15) << 1) & 0b11) {
            set(vec, vi, i, line[i >> 4] >> ((i & 15) << 1) & 0b11);
            changed++;
            line_remain[!vec][i]--;
            if (!(line_state[!vec][i >> 5] & 1 << (i & 31))) {
                state++;
                line_state[!vec][i >> 5] |= 1 << (i & 31);
            }
        }
    }
    remain -= changed;
    state--;
    line_remain[vec][vi] -= changed;
    line_state[vec][vi >> 5] &= ~(1 << (vi & 31));
}
bool Nonogram::solve(void) {
    while (true) {
        // Solve a line that can be updated.
        for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
            if (line_state[vec][vi >> 5] & 1 << (vi & 31)) {
                if (solveLine(vec, vi)) {
                    merge(vec, vi);
                } else {
                    return false;
                }
            }
        }

        // Verify or infer the table if no line can be updated.
        if (!state) {
            if (!remain) {
                if (verify()) {
                    // Copy the table to the base if the table is inferred.
                    if (this != &base) {
                        base.copy(this);
                    }
                    
                    return true;
                }
                return false;
            } else {
                return infer();
            }
        }
    }
}
bool Nonogram::infer(void) {
    // Infer a column line that is not full.
    for (uint16_t vi{0}; vi < size[1]; vi++) {
        if (line_remain[1][vi]) {
            return inferLine(1, vi);
        }
    }
    return false;
}
bool Nonogram::verify(void) {
    // Solve the table again to verify.
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        if (offset[vec][vi][1] != 1 && !solveLine(vec, vi)) {
            return false;
        }
    }
    return true;
}

uint16_t Nonogram::get(const uint16_t r, const uint16_t c) const {
    return table[r][c >> 4] >> ((c & 15) << 1) & 0b11;
}
uint16_t Nonogram::get(const bool vec, const uint16_t vi, const uint16_t i) const {
    return get(vec ? i : vi, vec ? vi : i);
}
void Nonogram::set(const uint16_t r, const uint16_t c, const uint16_t v) {
    table[r][c >> 4] |= v << ((c & 15) << 1);
}
void Nonogram::set(const bool vec, const uint16_t vi, const uint16_t i, const uint16_t v) {
    set(vec ? i : vi, vec ? vi : i, v);
}
void Nonogram::fill(const bool vec, const uint16_t vi, const uint16_t s, const uint16_t e, const uint16_t v) {
    for (uint16_t i{s}; i < e; i++) {
        if (!get(vec, vi, i)) {
            set(vec, vi, i, v);
        }
    }
}
void Nonogram::init(void) {
    // Initialize table, remain, state, line_remain and line_state.
    memset(table, 0, sizeof table);
    remain = size[0] * size[1];
    state = size[0] + size[1];
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        line_remain[vec][vi] = size[!vec];
    }
    memset(line_state, -1, sizeof line_state);
    // Fill lines whose clue is 0.
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        if (offset[vec][vi][1] == 1) {
            fill(vec, vi, 0, size[!vec], 0b01);
        }
    }
    // Fill lines whose clue is not 0.
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        if (offset[vec][vi][1] != 1) {
            for (uint16_t ci{0}; ci < clue_size[vec][vi]; ci++) {
                fill(vec, vi, offset[vec][vi][ci] + margin[vec][vi], offset[vec][vi][ci + 1] - 1, 0b10);
            }
        }
    }
    // Decrease remain if a pixel is not a gray.
    for (uint16_t r{0}; r < size[0]; r++) {
        for (uint16_t c{0}; c < size[1]; c++) {
            if (get(r, c)) {
                remain--;
                line_remain[0][r]--;
                line_remain[1][c]--;
            }
        }
    }
    // Set state to false if all pixels of a line are gray.
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        if (!line_remain[vec][vi] || line_remain[vec][vi] == size[!vec]) {
            state--;
            line_state[vec][vi >> 5] &= ~(1 << (vi & 31));
        }
    }
}
void Nonogram::copy(const Nonogram* const prev) {
    // Copy table, remain, state, line_remain and line_state
    memcpy(table, prev->table, sizeof table);
    remain = prev->remain;
    state = prev->state;
    memcpy(line_remain, prev->line_remain, sizeof line_remain);
    memcpy(line_state, prev->line_state, sizeof line_state);
}

void Nonogram::initLine(const uint16_t vec, const uint16_t vi) {
    // Initialize color.
    for (uint16_t i{0}; i < size[!vec]; i++) {
        color[0][i + 1] = color[0][i] + (get(vec, vi, i) == 0b01);
        color[1][i + 1] = color[1][i] + (get(vec, vi, i) == 0b10);
    }
    color[0][size[!vec] + 1] = color[0][size[!vec]];
    color[1][size[!vec] + 1] = color[1][size[!vec]];
    // Initialize shift.
    shift[0][0] = shift[0][1] = 0;
    memset(shift[1], 0, sizeof shift[1]);
    // Initialize cache and subcache.
    memset(cache[true][0], -1, sizeof cache[true][0]);
    memset(cache[true][clue_size[vec][vi] + 1], 0, sizeof cache[true][clue_size[vec][vi] + 1]);
    for (uint16_t ci{1}; ci <= clue_size[vec][vi]; ci++) {
        memset(cache[true][ci], 0, sizeof cache[true][ci]);
        memset(cache[false][ci], 0, sizeof cache[false][ci]);
        for (uint16_t si{0}; si <= margin[vec][vi]; si++) {
            if (color[0][offset[vec][vi][ci - 1] + si] != color[0][offset[vec][vi][ci] + si - 1] || color[1][offset[vec][vi][ci] + si - 1] != color[1][offset[vec][vi][ci] + si]) {
                cache[false][ci][si >> 5] |= 1 << (si & 31);
            }
        }
    }
    for (uint16_t si{0}; si <= margin[vec][vi]; si++) {
        if (color[1][offset[vec][vi][clue_size[vec][vi]] + si - 1] == color[1][size[!vec]]) {
            for (; si <= margin[vec][vi]; si++) {
                cache[true][clue_size[vec][vi] + 1][si >> 5] |= 1 << (si & 31);
            }
        } else {
            cache[false][clue_size[vec][vi]][si >> 5] |= 1 << (si & 31);
        }
    }
    memset(subcache, 0, sizeof subcache);
}
void Nonogram::fillLine(const uint16_t s, const uint16_t e, const uint16_t v) {    
    for (uint16_t i{s}; i < e; i++) {
        line[i >> 4] &= ~((0b11 ^ v) << ((i & 15) << 1));
    }
}
bool Nonogram::solveLine(const uint16_t vec, const uint16_t vi) {
    initLine(vec, vi);
    // Initialize line.
    memset(line, -1, sizeof line);
    for (uint16_t ci{1}; ci > 0;) {
        for (uint16_t &si{shift[0][ci]}; si <= margin[vec][vi]; si++) {
            // Proceed to next shift index if current one is impossible.
            if (cache[false][ci][si >> 5] & 1 << (si & 31)) {
                continue;
            }

            // Ignore the left shift index if the case that set 0b10 to 0b01 is found.
            if (color[1][offset[vec][vi][ci - 1] + shift[0][ci - 1]] != color[1][offset[vec][vi][ci - 1] + si]) {
                si = margin[vec][vi] + 1;

                break;
            }
            
            // Ignore current clue index if current shift index is known as a solution.
            if (cache[true][ci + 1][shift[0][ci] >> 5] & 1 << (shift[0][ci] & 31)) {
                ci++;
                shift[0][ci] = margin[vec][vi] + 1;
                subcache[ci >> 5] |= 1 << (ci & 31);

                break;
            }

            // Probe next clue index if shift index of current one is not probed and not impossible.
            if (color[0][offset[vec][vi][ci - 1] + si] == color[0][offset[vec][vi][ci] + si - 1]) {
                ci++;
                shift[0][ci] = shift[0][ci - 1];
                subcache[ci >> 5] &= ~(1 << (ci & 31));

                break;
            }
        }
        
        // Probe next shift index of previous clue index if current clue index is reached to the end of shift index.
        if (shift[0][ci] > margin[vec][vi]) {
            ci--;
            // Set previous shift index subcache to true if any solutions are found in current clue index.
            if (subcache[ci + 1 >> 5] & 1 << (ci + 1 & 31)) {
                subcache[ci >> 5] |= 1 << (ci & 31);
                // Set previous shift index to cache and fill the line of current clue index if previous shift index is not set to cache.
                // The order of setting a shift index to cache for each clue index is guaranteed to be in ascending order.
                if (!(cache[true][ci][shift[0][ci] >> 5] & 1 << (shift[0][ci] & 31))) {
                    if (!shift[1][ci]) {
                        fillLine(offset[vec][vi][ci - 1] + shift[0][ci - 1], offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                        fillLine(offset[vec][vi][ci - 1] + shift[0][ci], offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                    } else {
                        fillLine(std::max(offset[vec][vi][ci - 1] + shift[1][ci], offset[vec][vi][ci - 1] + shift[0][ci - 1]), offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                        fillLine(std::max(offset[vec][vi][ci] + shift[1][ci] - 1, offset[vec][vi][ci - 1] + shift[0][ci]), offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                    }
                    fillLine(offset[vec][vi][ci] + shift[0][ci] - 1, offset[vec][vi][ci] + shift[0][ci], 0b01);
                    shift[1][ci] = shift[0][ci];
                    cache[true][ci][shift[0][ci] >> 5] |= 1 << (shift[0][ci] & 31);
                }
            } else {
                cache[false][ci][shift[0][ci] >> 5] |= 1 << (shift[0][ci] & 31);
            }
            // Probe next shift index of previous clue index.
            shift[0][ci]++;
            
            continue;
        }
    }

    // Check if any solutions are found.
    if (subcache[0] & 1) {
        // Fill in the back of the smallest shift index of the last clue index with 0b01.
        for (uint16_t si{0};; si++) {
            if (cache[true][clue_size[vec][vi]][si >> 5] & 1 << (si & 31)) {
                fillLine(offset[vec][vi][clue_size[vec][vi]] + si, size[!vec], 0b01);

                break;
            }
        }

        return true;
    }
    return false;
}
bool Nonogram::inferLine(const uint16_t vec, const uint16_t vi) {
    initLine(vec, vi);
    for (uint16_t ci{1}; ci > 0;) {
        for (uint16_t &si{shift[0][ci]}; si <= margin[vec][vi]; si++) {
            // Proceed to next shift index if current one is impossible.
            if (cache[false][ci][si >> 5] & 1 << (si & 31)) {
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
                subcache[ci >> 5] &= ~(1 << (ci & 31));

                break;
            }
        }

        // Probe next shift index of previous clue index if current clue index is reached to the end of shift index.
        if (shift[0][ci] > margin[vec][vi]) {
            ci--;
            // Set previous shift index subcache to true if any solutions are found in current clue index.
            if (subcache[ci + 1 >> 5] & 1 << (ci + 1 & 31)) {
                subcache[ci >> 5] |= 1 << (ci & 31);
            } else {
                cache[false][ci][shift[0][ci] >> 5] |= 1 << (shift[0][ci] & 31);
            }
            // Probe next shift index of previous clue index.
            shift[0][ci]++;
            
            continue;
        }
        
        // Fill in the line and try to solve inferred table if clue index reached at the last and found a solution.
        if (ci == clue_size[vec][vi] + 1) {
            if (cache[true][ci][shift[0][ci - 1] >> 5] & 1 << (shift[0][ci - 1] & 31)) {
                memset(line, -1, sizeof line);
                for (ci = 1; ci <= clue_size[vec][vi]; ci++) {
                    fillLine(offset[vec][vi][ci - 1] + shift[0][ci - 1], offset[vec][vi][ci - 1] + shift[0][ci], 0b01);
                    fillLine(offset[vec][vi][ci - 1] + shift[0][ci], offset[vec][vi][ci] + shift[0][ci] - 1, 0b10);
                    fillLine(offset[vec][vi][ci] + shift[0][ci] - 1, offset[vec][vi][ci] + shift[0][ci], 0b01);
                }
                fillLine(offset[vec][vi][clue_size[vec][vi]] + shift[0][clue_size[vec][vi]], size[!vec], 0b01);
                
                if (Nonogram{this, vec, vi}.solve()) {
                    return true;
                }

                subcache[(ci - 1) >> 5] |= 1 << (ci - 1 & 31);
            }

            // Probe next shift index of previous clue index.
            ci--;
            shift[0][ci]++;
        }
    }
    
    return false;
}

bool Nonogram::solve(std::istream &is) {
    solubility = false;
    if (validity = input(is)) {
        base.init();
        solubility = base.solve();
    }

    return solubility;
}
bool Nonogram::isValid(void) {
    return validity;
}
bool Nonogram::isSolved(void) {
    return solubility;
}
void Nonogram::print(std::ostream &os) {
    for (uint16_t r{0}; r < size[0]; r++) {
        for (uint16_t c{0}; c < size[1]; c++) {
            os << printBinBit(base.get(r, c)) << ' ';
        }
        os << '\n';
    }
    os << '\n';
}
