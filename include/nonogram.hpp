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

    constexpr ng_size_t MAX_SIZE{255}; // Maximum table size (MAX_SIZE by MAX_SIZE matrix).
    constexpr ng_size_t MAX_CLUE_SIZE{MAX_SIZE + 1 >> 1}; // Maximum clue size.
    constexpr ng_size_t BIT_BUNDLE_SIZE{sizeof(ng_bit_array_t) << 3};

    // Return the size of a bit array corresponding to a size of a array.
    inline constexpr ng_size_t getBitArraySize(const ng_size_t size, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) return (size - 1) / BIT_BUNDLE_SIZE + 1;
        else if (bit_unit_size == 2) return (size - 1) / (BIT_BUNDLE_SIZE >> 1) + 1;
        else throw;
    }
    // Return a element of a bit array.
    inline constexpr ng_size_t getBitArrayElement(const ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) return bit_array[i >> 5] >> (i & 0b11111) & 0b1;
        else if (bit_unit_size == 2) return bit_array[i >> 4] >> ((i & 0b1111) << 1) & 0b11;
        else throw;
    }
    // Perform AND operations on a element of a bit array.
    inline constexpr void andBitArrayElement(ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t val, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) bit_array[i >> 5] &= ~((val ^ 0b1) << (i & 0b11111));
        else if (bit_unit_size == 2) bit_array[i >> 4] &= ~((val ^ 0b11) << ((i & 0b1111) << 1));
        else throw; 
    }
    // Perform OR operations on a element of a bit array.
    inline constexpr void orBitArrayElement(ng_bit_array_t* const bit_array, const ng_size_t i, const ng_size_t val, const ng_size_t bit_unit_size) {
        if (bit_unit_size == 1) bit_array[i >> 5] |= val << (i & 0b11111);
        else if (bit_unit_size == 2) bit_array[i >> 4] |= val << ((i & 0b1111) << 1);
        else throw; 
    }
    
    class Clue;
    class Solver;
    class LineSolver;
    
    // Nonogram clue class.
    class Clue {
        friend Solver;
        friend LineSolver;

    private:
        Clue(void) = default;
        Clue(const Clue&) = delete;
        Clue &operator=(const Clue&) = delete;
        ~Clue(void) = default;
        
        // Size of the table ([vector(0 row, 1 column)]).
        ng_size_t size_[2];
        // Prefix sum of the clue ([vector][vector index][clue element index]).
        ng_size_t offset_[2][MAX_SIZE][MAX_CLUE_SIZE + 1];
        // Size of the clue ([vector][vector index]).
        ng_size_t clue_size_[2][MAX_SIZE];
        // Margin based on size of the table and the clue ([vector][vector index]).
        ng_size_t margin_[2][MAX_SIZE];
        
        // Input the clue from array and validate, return validity.
        bool input(const ng_size_t* const);
    };
    
    // Nonogram solver class.
    class Solver {
        friend LineSolver;

#endif // NONOGRAM_HPP
