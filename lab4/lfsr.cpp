#include "lfsr.h"

LFSR::LFSR(uint32_t rec, uint8_t deg){
    recurrence = rec;
    degree = deg;
}

vector<uint8_t> LFSR::gen(uint32_t f, uint32_t length) {
    vector<uint8_t> seq;

    seq.reserve(length);

    uint32_t reg = f;

    for (uint32_t i = 0; i < length; i++) {
        uint8_t outBit = reg & 1;

        seq.push_back(outBit);

        uint8_t newBit = 0;

        uint32_t temp =
            reg & recurrence;

        while (temp) {
            newBit ^= (temp & 1);
            temp >>= 1;
        }

        reg >>= 1;

        reg |= (uint32_t(newBit)  << (degree - 1));
    }

    return seq;
}