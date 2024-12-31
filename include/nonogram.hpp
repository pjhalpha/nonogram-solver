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

    /**
     * @brief The maximum table size.
     *
     * The table is a `MAX_SIZE` by `MAX_SIZE` matrix.
     */
    constexpr ng_size_t MAX_SIZE{255};
    /**
     * @brief The maximum clue size.
     */
    constexpr ng_size_t MAX_CLUE_SIZE{MAX_SIZE + 1 >> 1};
    /**
     * @brief The size of the integer data type to store the unit bits.
     */
    constexpr ng_size_t BIT_BUNDLE_SIZE{sizeof(ng_bit_array_t) << 3};

    /**
     * @brief Return the size of a bit array corresponding to `size`.
     *
     * @param size The expected size of the total array.
     * @param bit_unit_size The size of a unit bit. It must be 1 or 2.
     *
     * @return The size of a bit array corresponding to `size`.
     */
    inline constexpr ng_size_t getBitArraySize(const ng_size_t size, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) {
            return (size - 1) / BIT_BUNDLE_SIZE + 1;
        } else if (bit_unit_size == 2) {
            return (size - 1) / (BIT_BUNDLE_SIZE >> 1) + 1;
        } else throw;
    }
    /**
     * @brief Return an element of the bit array.
     *
     * @param bit_array The bit array.
     * @param i Index of the array.
     * @param bit_unit_size The size of a unit bit. It must be 1 or 2.
     *
     * @return The value of the bit array.
     */
    inline constexpr ng_size_t getBitArrayElement(const ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) {
            return bit_array[i >> 5] >> (i & 0b11111) & 0b1;
        } else if (bit_unit_size == 2) {
            return bit_array[i >> 4] >> ((i & 0b1111) << 1) & 0b11;
        } else throw;
    }
    /**
     * @brief Perform AND operations on an element of the bit array.
     *
     * @param bit_array The bit array.
     * @param i Index of the array.
     * @param val The value with which an element in the array will be ANDed.
     * @param bit_unit_size The size of a unit bit. It must be 1 or 2.
     *
     * @return The value of the bit array.
     */
    inline constexpr void andBitArrayElement(ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t val, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) {
            bit_array[i >> 5] &= ~((val ^ 0b1) << (i & 0b11111));
        } else if (bit_unit_size == 2) {
            bit_array[i >> 4] &= ~((val ^ 0b11) << ((i & 0b1111) << 1));
        } else throw;
    }
    /**
     * @brief Perform OR operations on an element of the bit array.
     *
     * @param bit_array The bit array.
     * @param i Index of the array.
     * @param val The value with which an element in the array will be ORed.
     * @param bit_unit_size The size of a unit bit. It must be 1 or 2.
     *
     * @return The value of the bit array.
     */
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
