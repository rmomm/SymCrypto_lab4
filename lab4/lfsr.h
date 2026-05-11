#pragma once
#include <vector>
#include <iostream>

using namespace std;

class LFSR {
private:
    vector<int> state;   
    vector<int> taps;    

public:
    LFSR(const vector<int>& initSt, const vector<int>& tapP);

    int step();

    vector<int> generate(int n);

    vector<int> getState() const;
};