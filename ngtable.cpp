#include "nonogram.hpp"

const uint16_t (&NonogramTable::size)[2]{Nonogram::size};
const uint16_t (&NonogramTable::offset)[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1]{Nonogram::offset};
const uint16_t (&NonogramTable::clue_size)[2][MAX_NG_SIZE]{Nonogram::clue_size};
const uint16_t (&NonogramTable::margin)[2][MAX_NG_SIZE]{Nonogram::margin};

NonogramTable::NonogramTable(NonogramLine* const line) : line{line} {
}

uint16_t NonogramTable::get(const bool vec, const uint16_t vi, const uint16_t i) const noexcept {
    return getBinBit(base[vec ? i : vi], vec ? vi : i);
}
void NonogramTable::set(const bool vec, const uint16_t vi, const uint16_t i, const uint16_t v) {
    setBinBit(base[vec ? i : vi], vec ? vi : i, v);
}
void NonogramTable::init(void) {
    for (uint16_t vi{0}; vi < size[0]; vi++) {
        base[vi].reset();
    }
    remain = size[0] * size[1];
    state = size[0] + size[1];
    for (uint16_t vec{0}; vec < 2; vec++) {
        std::fill(&line_remain[vec][0], &line_remain[vec][0] + size[vec], size[!vec]);
        line_state[vec].set();
    }
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
            if (getBinBit(base[r], c) != 0b00) {
                remain--;
                line_remain[0][r]--;
                line_remain[1][c]--;
            }
        }
    }
    // Set state to false if all pixels of a line is gray.
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        if (line_remain[vec][vi] == 0 || line_remain[vec][vi] == size[!vec]) {
            state--;
            line_state[vec].reset(vi);
        }
    }
}
void NonogramTable::fill(const bool vec, const uint16_t vi, const uint16_t begin, const uint16_t end, const uint16_t v) {
    for (uint16_t i{begin}; i < end; i++) {
        if (get(vec, vi, i) == 0b00) {
            set(vec, vi, i, v);
        }
    }
}
void NonogramTable::merge(const uint16_t vec, const uint16_t vi) {
    uint16_t changed{0};

    // Merge the table with the solved line and update variable.
    for (uint16_t i{0}; i < size[!vec]; i++) {
        if (get(vec, vi, i) == 0b00 && getBinBit(line->base, i) != 0b00) {
            set(vec, vi, i, getBinBit(line->base, i));
            changed++;
            line_remain[!vec][i]--;
            if (!line_state[!vec].test(i)) {
                state++;
                line_state[!vec].set(i);
            }
        }
    }
    remain -= changed;
    state--;
    line_remain[vec][vi] -= changed;
    line_state[vec].reset(vi);
}
bool NonogramTable::verify(void) {
    // Solve the table again to verify.
    for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
        if (offset[vec][vi][1] != 1 && !line->solve(vec, vi)) {
            return false;
        }
    }
    return true;
}
void NonogramTable::copy(const NonogramTable &table) {
    std::copy(table.base, table.base + MAX_NG_SIZE, base);
    remain = table.remain;
    state = table.state;
    std::copy(&table.line_remain[0][0], &table.line_remain[0][0] + 2 * MAX_NG_SIZE, &line_remain[0][0]);
    std::copy(table.line_state, table.line_state + 2, line_state);
}
