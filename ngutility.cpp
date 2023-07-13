#include "ngutility.hpp"

void getLineStream(std::istream &is, std::stringstream &ss) {
    static std::string buf;
    
    std::getline(is, buf);
    ss = std::stringstream(buf);
}
uint16_t getBinBit(const std::bitset<MAX_NG_SIZE << 1> &bs, const uint16_t i) noexcept {
    return i >= MAX_NG_SIZE ? 0b11 : (bs.test((i << 1) + 1) << 1) | bs.test(i << 1);
}
void setBinBit(std::bitset<MAX_NG_SIZE << 1> &bs, const uint16_t i, const uint16_t v) {
    bs.set((i << 1) + 1, v & 0b10);
    bs.set(i << 1, v & 0b01);
}
void fillBinBit(std::bitset<MAX_NG_SIZE << 1> &bs, const uint16_t begin, const uint16_t end, const uint16_t v) {
    for (uint16_t i{begin}; i < end; i++) {
        setBinBit(bs, i, v);
    }
}
char printBinBit(const uint16_t v) {
    return v == 0b10 ? '#' : v == 0b01 ? '.' : v == 0b00 ? '?' : '!';
}
