#include "lfsr.h"

LFSR::LFSR(const vector<int>& initState, const vector<int>& tapPositions) {
    state = initState;
    taps = tapPositions;
}

int LFSR::step() {
    int output = state[0];

    int newBit = 0;

    for (int tap : taps) {
        newBit ^= state[tap];
    }

    for (size_t i = 0; i < state.size() - 1; i++) {
        state[i] = state[i + 1];
    }

    state[state.size() - 1] = newBit;

    return output;
}

vector<int> LFSR::generate(int length) {
    vector<int> sequence;

    for (int i = 0; i < length; i++) {
        sequence.push_back(step());
    }

    return sequence;
}

vector<int> LFSR::getState() const {
    return state;
}