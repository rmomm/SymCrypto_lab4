#include "geffe.h"

Geffe::Geffe(const LFSR& r1,  const LFSR& r2,const LFSR& r3): l1(r1), l2(r2), l3(r3)
{ }

int Geffe::step() {
    int x = l1.step();
    int y = l2.step();
    int s = l3.step();

    int z = (s & x) ^ ((1 ^ s) & y);

    return z;
}

vector<int> Geffe::generate(int length) {
    vector<int> sequence;

    for (int i = 0; i < length; i++) {
        sequence.push_back(step());
    }

    return sequence;
}