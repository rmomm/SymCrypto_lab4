#include "geffe.h"

Geffe::Geffe(const LFSR& r1,const LFSR& r2, const LFSR& r3) : l1(r1), l2(r2), l3(r3)
{}

vector<uint8_t> Geffe::generate(uint32_t f1, uint32_t f2, uint32_t f3,int length) {
    vector<uint8_t> x =  l1.gen(f1, length);
    vector<uint8_t> y = l2.gen(f2, length);
    vector<uint8_t> s = l3.gen(f3, length);
    vector<uint8_t> z(length);

    for (int i = 0; i < length; i++) {
        z[i] = (s[i] & x[i]) ^ ((1 ^ s[i]) & y[i]);
    }

    return z;
}