#ifndef PJHALPHA_NGENV_H
#define PJHALPHA_NGENV_H

#include <iostream>
#include <fstream>
#include <sstream>
#include "ngutility.hpp"

#define MAX_NG_SIZE 255 // Maximum table size, expressed using 2 bytes (MAX_NG_SIZE by MAX_NG_SIZE matrix).
#define MAX_NG_CLUE_SIZE (MAX_NG_SIZE + 1 >> 1) // Maximum clue size.
static_assert(MAX_NG_SIZE < 0x10000);

// Forward declaration.
class Nonogram; // Overall flow manager class.
class NonogramTable; // Table manager class.
class NonogramLine; // Line manager class.

#endif // PJHALPHA_NGENV_H
