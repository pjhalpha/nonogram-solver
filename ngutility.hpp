#ifndef PJHALPHA_NGUTILITY_H
#define PJHALPHA_NGUTILITY_H

#include "ngenv.hpp"

void getLineStream(std::istream&, std::stringstream&); // Get stringstream of a line from istream.
char printBinBit(const uint16_t); // Return character for a pixel.

#endif // PJHALPHA_NGUTILITY_H
