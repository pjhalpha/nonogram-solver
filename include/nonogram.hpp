#ifndef NONOGRAM_HPP
#define NONOGRAM_HPP

#include <iostream>
#include <cstring>
#include <ctime>
#include <cstdint>

namespace Nonogram {
    typedef uint16_t ng_size_t;
    typedef uint32_t ng_sq_size_t;
    typedef uint32_t ng_bit_array_t;

    // Maximum table size (MAX_SIZE by MAX_SIZE matrix).
    constexpr ng_size_t MAX_SIZE{255};
    // Maximum clue size.
    constexpr ng_size_t MAX_CLUE_SIZE{MAX_SIZE + 1 >> 1};
    // The size of the integer data type to store the unit bits.
    constexpr ng_size_t BIT_BUNDLE_SIZE{sizeof(ng_bit_array_t) << 3};

    // Return the size of a bit array corresponding to a size of a array.
    inline constexpr ng_size_t getBitArraySize(const ng_size_t size, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) {
            return (size - 1) / BIT_BUNDLE_SIZE + 1;
        } else if (bit_unit_size == 2) {
            return (size - 1) / (BIT_BUNDLE_SIZE >> 1) + 1;
        } else throw;
    }
    // Return a element of a bit array.
    inline constexpr ng_size_t getBitArrayElement(const ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) {
            return bit_array[i >> 5] >> (i & 0b11111) & 0b1;
        } else if (bit_unit_size == 2) {
            return bit_array[i >> 4] >> ((i & 0b1111) << 1) & 0b11;
        } else throw;
    }
    // Perform AND operations on a element of a bit array.
    inline constexpr void andBitArrayElement(ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t val, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) {
            bit_array[i >> 5] &= ~((val ^ 0b1) << (i & 0b11111));
        } else if (bit_unit_size == 2) {
            bit_array[i >> 4] &= ~((val ^ 0b11) << ((i & 0b1111) << 1));
        } else throw;
    }
    // Perform OR operations on a element of a bit array.
    inline constexpr void orBitArrayElement(ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t val, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) {
            bit_array[i >> 5] |= val << (i & 0b11111);
        } else if (bit_unit_size == 2) {
            bit_array[i >> 4] |= val << ((i & 0b1111) << 1);
        } else throw;
    }
};

#include "line_clue.hpp"
#include "clue.hpp"
#include "line_solver.hpp"
#include "solver.hpp"

#endif // NONOGRAM_HPP
