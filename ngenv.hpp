#ifndef PJHALPHA_NGENV_H
#define PJHALPHA_NGENV_H

#include <iostream>
#include <fstream>
#include <exception>
#include <sstream>
#include <bitset>

#define MAX_NG_SIZE 255 // Maximum table size (MAX_NG_SIZE by MAX_NG_SIZE matrix).
#define MAX_NG_CLUE_SIZE (MAX_NG_SIZE + 1 >> 1) // Maximum clue size based on table size.

// MAX_NG_SIZE should be expressed in 2 bytes, and uint16_t and uint32_t able to afford at least 2 bytes and 4 bytes respectively.
static_assert(MAX_NG_SIZE < 0x10000 && sizeof(uint16_t) >= 2 && sizeof(uint32_t) >= 4);

#include "ngutility.hpp"

class Nonogram; // Overall flow manager class.
class NonogramTable; // Table manager class.
class NonogramLine; // Line manager class.

#endif
