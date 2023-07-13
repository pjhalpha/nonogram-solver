#ifndef PJHALPHA_NGUTILITY_H
#define PJHALPHA_NGUTILITY_H

#include "ngenv.hpp"

void getLineStream(std::istream&, std::stringstream&); // Get stringstream of a line from istream.
uint16_t getBinBit(const std::bitset<MAX_NG_SIZE << 1>&, const uint16_t) noexcept; // Get a binary bit of bitset.
void setBinBit(std::bitset<MAX_NG_SIZE << 1>&, const uint16_t, const uint16_t); // Set a binary bit of bitset.
void fillBinBit(std::bitset<MAX_NG_SIZE << 1>&, const uint16_t, const uint16_t, const uint16_t); // Fill binary bits of bitset from the begin to the end.
char printBinBit(const uint16_t); // Return character of a binary bit.

#endif
