#pragma once
#include "lfsr.h"
#include <vector>

using namespace std;

class Geffe {
private:
    LFSR l1;
    LFSR l2;
    LFSR l3;

public:
    Geffe(const LFSR& r1, const LFSR& r2, const LFSR& r3);

    int step();

    vector<int> generate(int length);
};