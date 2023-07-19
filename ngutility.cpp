#include "ngutility.hpp"

void getLineStream(std::istream &is, std::stringstream &ss) {
    static std::string buf;
    
    std::getline(is, buf);
    ss = std::stringstream(buf);
}
char printBinBit(const uint16_t v) {
    return v == 0b10 ? '#' : v == 0b01 ? '.' : v == 0b00 ? '?' : '!';
}
