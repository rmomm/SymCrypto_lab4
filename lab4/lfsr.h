#pragma once

#include <vector>
#include <cstdint>

using namespace std;

class LFSR {
private:

    uint32_t recurrence;
    uint8_t degree;

public:

    LFSR(uint32_t rec, uint8_t deg);
    vector<uint8_t> gen(uint32_t f, uint32_t length);
};