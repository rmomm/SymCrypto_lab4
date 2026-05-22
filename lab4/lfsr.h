#pragma once
#include <vector>

using namespace std;

class LFSR {
private:
    vector<int> state;
    vector<int> taps;

public:
    LFSR(const vector<int>& initState, const vector<int>& tapPositions);

    int step();

    vector<int> generate(int length);
    vector<int> getState() const;
};