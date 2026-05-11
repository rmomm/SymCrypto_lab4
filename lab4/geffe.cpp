#include "geffe.h"

Geffe::Geffe(LFSR l1, LFSR l2, LFSR l3): L1(l1), L2(l2), L3(l3) {

}

vector<int> Geffe::generate(int n) {
    vector<int> z;
    z.reserve(n);

    for (int i = 0; i < n; i++) {
        int x = L1.step();
        int y = L2.step();
        int s = L3.step();


        int zi = (s == 1) ? x : y;

        z.push_back(zi);
    }

    return z;
}