#pragma once
#include <vector>
#include "lfsr.h"

using namespace std;

class Geffe {
private:
    LFSR L1;
    LFSR L2;
    LFSR L3;

public:
    Geffe(LFSR l1, LFSR l2, LFSR l3);

    vector<int> generate(int n);
};