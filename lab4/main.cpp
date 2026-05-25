#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "lfsr.h"
#include "geffe.h"

using namespace std;


string loadSequence(const string& filename) {
    ifstream f(filename);
    string s;
    f >> s;
    return s;
}


vector<int> toBits(const string& s) {
    vector<int> bits;
    for (char c : s) {
        bits.push_back(c - '0');
    }

    return bits;
}


void printState(ostream& out, const vector<int>& state){
    for (int b : state) {
        out << b;
    }

    out << endl;
}


double calcC(int N) {
    double p1 = 0.25;
    double t = 2.33;

    return N * p1 + t * sqrt(N * p1 * (1.0 - p1));
}


double calcBeta(int N, double C) {
    double p2 = 0.5;
    double t = (N * p2 - C) / sqrt(N * p2 * (1.0 - p2));
    double beta =0.5 * erfc(t / sqrt(2.0));
    return beta;
}


int calcNstar(int l) {
    double M = pow(2.0, l);

    for (int N = 50; N <= 5000; N++) {
        double C = calcC(N);

        double beta = calcBeta(N, C);

        if (beta * M < 1.0) {
            return N;
        }
    }

    return 200;
}


int computeR(const vector<int>& a, const vector<int>& b) {
    int R = 0;

    for (int i = 0; i < (int)a.size(); i++) {
        R += (a[i] ^ b[i]);
    }

    return R;
}


vector<int> attackL1(const vector<int>& z,  int N, double C, ostream& out, ostream& candOut) {
    vector<int> taps = { 0, 3};

    int bestR = N + 1;

    vector<int> bestState;

    int candidates = 0;

    out << "\n===== CORRELATION ATTACK ON L1 =====\n";

    cout << "\nSearching L1...\n";

    for (int mask = 1; mask < (1 << 20); mask++) {
        vector<int> state(25);

        for (int i = 0; i < 25; i++) {
            state[24 - i] =  (mask >> i) & 1;
        }

        LFSR reg(state, taps);

        vector<int> x = reg.generate(N);

        int R = computeR(x, z);

        if (R < C) {
            candidates++;
            candOut << "L1 candidate:\n";

            printState(candOut, state);

            candOut << "R = " << R  << "\n\n";
        }

        if (R < bestR) {
            bestR = R;

            bestState = state;

            cout << "L1 new best R = "  << bestR   << endl;
        }
    }

    out << "\nCandidates found: "  << candidates  << endl;

    out << "\nRecovered L1:\n";

    printState(out, bestState);

    out << "Best R = " << bestR << endl;

    return bestState;
}


vector<int> attackL2(const vector<int>& z,  int N, double C,  ostream& out,  ostream& candOut) {
    vector<int> taps = { 0, 1, 2, 6};

    int bestR = N + 1;

    vector<int> bestState;

    int candidates = 0;

    out << "\n===== CORRELATION ATTACK ON L2 =====\n";

    cout << "\nSearching L2...\n";

    for (int mask = 1;  mask < (1 << 20); mask++) {
        vector<int> state(26);

        for (int i = 0; i < 26; i++) {
            state[25 - i] = (mask >> i) & 1;
        }

        LFSR reg(state, taps);

        vector<int> y = reg.generate(N);

        int R = computeR(y, z);

        if (R < C) {
            candidates++;

            candOut << "L2 candidate:\n";

            printState(candOut, state);

            candOut << "R = " << R << "\n\n";
        }

        if (R < bestR) {
            bestR = R;

            bestState = state;

            cout << "L2 new best R = " << bestR << endl;
        }
    }

    out << "\nCandidates found: " << candidates << endl;

    out << "\nRecovered L2:\n";

    printState(out, bestState);

    out << "Best R = " << bestR << endl;

    return bestState;
}


vector<int> attackL3(const vector<int>& z, const vector<int>& x, const vector<int>& y, int N, ostream& out) {
    vector<int> taps = { 0,1,2,5 };

    int bestMatches = -1;

    vector<int> bestState;

    out << "\n===== ATTACK ON L3 =====\n";

    cout << "\nSearching L3...\n";

    for (unsigned long long mask = 1; mask < (1ULL << 20); mask++) {
        vector<int> state(27);

        for (int i = 0; i < 27; i++) {
            state[26 - i] = (mask >> i) & 1;
        }

        LFSR reg(state, taps);

        vector<int> s = reg.generate(N);

        int matches = 0;

        for (int i = 0; i < N; i++) {
            if (x[i] != y[i]) {
                if (s[i] == 1 && z[i] == x[i]) {
                    matches++;
                }

                if (s[i] == 0 && z[i] == y[i]) {
                    matches++;
                }
            }
        }

        if (matches > bestMatches) {
            bestMatches = matches;

            bestState = state;

            cout << "L3 best matches = " << bestMatches << endl;
        }
    }

    out << "\nRecovered L3:\n";

    printState(out, bestState);

    out << "Matches = " << bestMatches << endl;

    return bestState;
}


void verifyKey(const vector<int>& L1state, const vector<int>& L2state, const vector<int>& L3state, const vector<int>& z, int N,  ostream& out) {
    LFSR l1(L1state, { 0,3 });
    LFSR l2(L2state, { 0,1,2,6 });
    LFSR l3( L3state, { 0,1,2,5 });

    Geffe geffe(l1, l2, l3);

    vector<int> generated = geffe.generate(N);

    int errors = 0;

    for (int i = 0; i < N; i++) {
        if (generated[i] != z[i]) {
            errors++;
        }
    }

    out << "\n===== KEY VERIFICATION =====\n";

    out << "Errors = " << errors << endl;

    if (errors == 0) {
        out << "SUCCESS\n";
    }
    else {
        out << "FAILED\n";
    }
}


int main() {
    ofstream out("results.txt");

    ofstream candOut("candidates.txt");

    string s = loadSequence("v12.txt");

    vector<int> z = toBits(s);


    int l1 = 25;
    int l2 = 26;
    int N1 = calcNstar(l1);
    int N2 = calcNstar(l2);
    double C1 = calcC(N1);
    double C2 = calcC(N2);

    vector<int> z1(z.begin(), z.begin() + N1);
    vector<int> z2(z.begin(), z.begin() + N2);


    cout << "Loaded sequence length: "<< z.size()<< endl;

    cout << "\nL1:\n";
    cout << "N* = " << N1 << endl;
    cout << "C = " << C1 << endl;
    cout << "\nL2:\n";
    cout << "N* = " << N2 << endl;
    cout << "C = " << C2 << endl;

    out << "\n===== SEQUENCE z =====\n";

    for (int b : z) {
        out << b;
    }

    out << endl;

    out << "\n===== PARAMETERS N* AND C =====\n";

    out << "\nL1:\n";
    out << "N* = " << N1  << endl;
    out << "C = " << C1  << endl;
    out << "\nL2:\n";
    out << "N* = " << N2 << endl;
    out << "C = "   << C2  << endl;

    vector<int> bestL1 = attackL1(z1, N1, C1, out, candOut);
    vector<int> bestL2 = attackL2(z2, N2, C2, out, candOut);

    int N3 = min(N1, N2);

    LFSR reg1( bestL1, { 0,3 });
    LFSR reg2( bestL2, { 0,1,2,6 });

    vector<int> x = reg1.generate(N3);
    vector<int> y = reg2.generate(N3);
    vector<int> z3(z.begin(), z.begin() + N3);

    vector<int> bestL3 = attackL3( z3, x, y, N3, out);

    verifyKey(bestL1, bestL2, bestL3, z3, N3, out);

    out << "\n===== FINAL RESULTS =====\n";
    out << "\nRecovered L1:\n";
    printState(out, bestL1);
    out << "\nRecovered L2:\n";
    printState(out, bestL2);
    out << "\nRecovered L3:\n";
    printState(out, bestL3);

    out.close();
    candOut.close();

    return 0;
}