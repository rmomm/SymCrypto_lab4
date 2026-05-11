#include "lfsr.h"

LFSR::LFSR(const vector<int>& initSt, const vector<int>& tapP) {
    state = initSt;
    taps = tapP;
}

int LFSR::step() {
    int output = state[0];
    int newBit = 0;

    for (int t : taps) {
        newBit ^= state[t];
    }

    for (size_t i = 0; i < state.size() - 1; i++) {
        state[i] = state[i + 1];
    }

    state[state.size() - 1] = newBit;

    return output;
}

vector<int> LFSR::generate(int n) {
    vector<int> r;
    r.reserve(n);

    for (int i = 0; i < n; i++) {
        r.push_back(step());
    }

    return r;
}

vector<int> LFSR::getState() const {
    return state;
}

