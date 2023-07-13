#include "nonogram.hpp"

Nonogram::Nonogram(void) : table{&line}, line{&table} {
}
Nonogram::Nonogram(const Nonogram* const ng, const uint16_t vec, const uint16_t vi) : Nonogram{} {
    table.copy(ng->table);
    table.state++;
    line.base = ng->line.base;
    table.merge(vec, vi);
}

Nonogram Nonogram::base;
uint16_t Nonogram::size[2];
uint16_t Nonogram::offset[2][MAX_NG_SIZE][MAX_NG_CLUE_SIZE + 1];
uint16_t Nonogram::clue_size[2][MAX_NG_SIZE];
uint16_t Nonogram::margin[2][MAX_NG_SIZE];
bool Nonogram::validity;
bool Nonogram::solubility;
bool Nonogram::input(std::istream &is) {
    std::stringstream ls;
    uint16_t clue_sum[2]{0};

    // Get the number of rows and columns of the table.
    getLineStream(is, ls);
    ls >> size[0] >> size[1];
    
    if (ls >> size[0]) return false;

    // Get the clue and initialize offset, clue_size and margin.
    for (uint16_t vec{0}; vec < 2; vec++) {
        for (uint16_t vi{0}; vi < size[vec]; vi++) {
            uint16_t &k{clue_size[vec][vi]};

            getLineStream(is, ls);
            for (k = 0; k < MAX_NG_CLUE_SIZE && ls >> offset[vec][vi][k + 1]; k++) {
                offset[vec][vi][k + 1] += offset[vec][vi][k] + 1;
            }

            if (k == 0 || ls >> offset[vec][vi][k + 1] || k != 1 && offset[vec][vi][1] == 0) return false;

            margin[vec][vi] = size[!vec] - offset[vec][vi][k] + 1;
            clue_sum[vec] += offset[vec][vi][k] - k;
        
            if (margin[vec][vi] < 0) return false;
        }
    }
    return clue_sum[0] == clue_sum[1];
}

bool Nonogram::solve(void) {
    while (true) {
        // Solve a line that can be updated in the order of row and column.
        for (uint16_t vec{0}; vec < 2; vec++) for (uint16_t vi{0}; vi < size[vec]; vi++) {
            if (table.line_state[vec].test(vi)) {
                if (line.solve(vec, vi)) {
                    table.merge(vec, vi);
                } else {
                    return false;
                }
            }
        }

        // Verify or infer the table if no line can be updated.
        if (table.state == 0) {
            if (table.remain == 0) {
                if (table.verify()) {
                    // Copy the table to the base if the table is inferred.
                    if (this != &base) {
                        base.table.copy(this->table);
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
    for (uint16_t vi{}; vi < size[1]; vi++) {
        // Infer a column line that is not full.
        if (table.line_remain[1][vi] > 0) {
            return line.infer(this, 1, vi);
        }
    }

    return false;
}

bool Nonogram::solve(std::istream &is) {
    solubility = false;
    if (validity = input(is)) {
        base.table.init();
        solubility = base.solve();
    }

    return solubility;
}
bool Nonogram::isValid(void) noexcept {
    return validity;
}
bool Nonogram::isSolved(void) noexcept {
    return solubility;
}
void Nonogram::print(std::ostream &os) {
    for (uint16_t r{0}; r < size[0]; r++) {
        for (uint16_t c{0}; c < size[1]; c++) {
            os << printBinBit(getBinBit(base.table.base[r], c)) << ' ';
        }
        os << '\n';
    }
    os << '\n';
}
