#pragma once

#include "lfsr.h"

class Geffe
{
private:

    LFSR l1;
    LFSR l2;
    LFSR l3;

public:

    Geffe(const LFSR& r1, const LFSR& r2, const LFSR& r3);

    vector<uint8_t> generate(uint32_t f1, uint32_t f2, uint32_t f3, int length);
};