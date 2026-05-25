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

vector<uint8_t> toBits(const string& s) {
    vector<uint8_t> bits;

    for (char c : s) {
        bits.push_back(c - '0');
    }

    return bits;
}


double calcC(int N) {
    double p1 = 0.25;
    double t = 2.326347874;

    return N * p1 + t * sqrt(N * p1 * (1.0 - p1));
}

double calcBeta(int N, double C) {
    double p2 = 0.5;
    double t = (N * p2 - C) / sqrt(N * p2 * (1.0 - p2));
    double beta = 0.5 * erfc(t / sqrt(2.0));
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

void printBits(ostream& out, uint32_t value, int len) {
    for (int i = len - 1; i >= 0; i--) {
        out << ((value >> i) & 1);
    }

    out << endl;
}


int computeR(const vector<uint8_t>& a, const vector<uint8_t>& b, int N){
    int R = 0;

    for (int i = 0; i < N; i++) {
        R += (a[i] ^ b[i]);
    }

    return R;
}

uint32_t nextState(uint32_t st, uint32_t bit, int deg) {
    st = st >> 1;
    st = st ^ (bit << (deg - 1));
    return st;
}


void attackL1(LFSR& L1, const vector<uint8_t>& z, int N1, double C1, int L1_deg, vector<pair<uint32_t, int>>& L1_candidates, uint32_t& bestL1, int& bestR1, ofstream& candOut) {

    uint64_t c = (1ULL << L1_deg) + N1;
    uint32_t cur = 1u;
    auto seq = L1.gen(cur, c);

    for (uint64_t j = 0; j < (1ULL << L1_deg); j++) {

        int R = 0;

        for (int i = 0; i < N1; i++)
            R += (seq[j + i] ^ z[i]);

        if (R < C1) {

            L1_candidates.push_back({ cur, R });
            candOut << "L1 candidate:\n";
            printBits(candOut, cur, L1_deg);
            candOut << "R = " << R << "\n\n";
        }

        if (R < bestR1) {
            bestR1 = R;
            bestL1 = cur;
            cout << "L1 best R = "  << bestR1  << endl;
        }

        uint32_t bit = seq[L1_deg + j];
        cur = nextState(cur, bit, L1_deg);
    }
}


void attackL2(LFSR& L2, const vector<uint8_t>& z, int N2, double C2, int L2_deg, vector<pair<uint32_t, int>>& L2_candidates,  uint32_t& bestL2, int& bestR2, ofstream& candOut) {

    uint64_t c = (1ULL << L2_deg) + N2;
    uint32_t cur = 1u;
    auto seq = L2.gen(cur, c);

    for (uint64_t j = 0; j < (1ULL << L2_deg); j++) {

        int R = 0;

        for (int i = 0; i < N2; i++)
            R += (seq[j + i] ^ z[i]);

        if (R < C2) {
            L2_candidates.push_back({ cur, R });
            candOut << "L2 candidate:\n";
            printBits(candOut, cur, L2_deg);
            candOut << "R = " << R << "\n\n";
        }

        if (R < bestR2) {
            bestR2 = R;
            bestL2 = cur;
            cout << "L2 best R = " << bestR2 << endl;
        }

        uint32_t bit = seq[L2_deg + j];

        cur = nextState(cur, bit, L2_deg);
    }
}


void attackL3(LFSR& L1, LFSR& L2,  LFSR& L3, const vector<uint8_t>& z, uint32_t bestL1, uint32_t bestL2, int L3_deg, uint32_t& bestL3) {
    auto x = L1.gen(bestL1, z.size());
    auto y = L2.gen(bestL2, z.size());
    uint64_t c = (1ULL << L3_deg) + z.size();
    auto seq = L3.gen(1u, c);
    uint32_t cur = 1u;

    for (uint64_t j = 0; j < (1ULL << L3_deg); j++) {

        bool ok = true;

        for (size_t i = 0; i < z.size(); i++) {

            uint8_t bit = (seq[j + i] & x[i]) ^ ((1 ^ seq[j + i]) & y[i]);

            if (bit != z[i]) {
                ok = false;
                break;
            }
        }

        if (ok) {
            bestL3 = cur;
            cout << "L3 found!\n";
            return;
        }

        uint32_t bit = seq[L3_deg + j];
        cur = nextState(cur, bit, L3_deg);
    }

    cout << "L3 not found\n";
}

int verifyKey(Geffe& geffe, const vector<uint8_t>& z, uint32_t bestL1, uint32_t bestL2, uint32_t bestL3) {
    auto seq =  geffe.generate(bestL1, bestL2,  bestL3, z.size());
    int err = 0;

    for (size_t i = 0; i < z.size(); i++) {
        if (seq[i] != z[i])
            err++;
    }

    return err;
}


int main() {

    ofstream out("results9.txt");
    ofstream candOut("candidates9.txt");

    uint32_t L1_rec = ((1u << 3) ^ 1u);
    uint32_t L2_rec = ((1u << 6) ^ (1u << 2) ^ (1u << 1) ^ 1u);
    uint32_t L3_rec = ((1u << 5) ^ (1u << 2) ^ (1u << 1) ^ 1u);

    constexpr uint8_t L1_deg = 25;
    constexpr uint8_t L2_deg = 26;
    constexpr uint8_t L3_deg = 27;

    string s = loadSequence("v9.txt");

    vector<uint8_t> z = toBits(s);

    int N1 = calcNstar(25);
    int N2 = calcNstar(26);

    double C1 = calcC(N1);
    double C2 = calcC(N2);

    out << "===== PARAMETERS =====\n";

    out << "\nL1:\n";
    out << "N* = " << N1 << endl;
    out << "C = " << C1 << endl;

    out << "\nL2:\n";
    out << "N* = " << N2 << endl;
    out << "C = " << C2 << endl;

    LFSR L1(L1_rec, L1_deg);
    LFSR L2(L2_rec, L2_deg);
    LFSR L3(L3_rec, L3_deg);

    vector<pair<uint32_t, int>> L1_candidates;
    uint32_t bestL1 = 0;
    int bestR1 = N1 + 1;
    cout << "\nSearching L1...\n";
    attackL1(L1, z, N1, C1, L1_deg, L1_candidates, bestL1, bestR1, candOut);

    out << "\n===== CORRELATION ATTACK ON L1 =====\n";
    out << "\nCandidates found: " << L1_candidates.size() << endl;
    out << "\nRecovered L1:\n";
    printBits(out, bestL1, L1_deg);
    out << "Best R = "  << bestR1  << endl;


    vector<pair<uint32_t, int>> L2_candidates;
    uint32_t bestL2 = 0;
    int bestR2 = N2 + 1;
    cout << "\nSearching L2...\n";
    attackL2(L2, z, N2, C2, L2_deg, L2_candidates, bestL2, bestR2, candOut);

    out << "\n===== CORRELATION ATTACK ON L2 =====\n";
    out << "\nCandidates found: "  << L2_candidates.size() << endl;
    out << "\nRecovered L2:\n";
    printBits(out, bestL2, L2_deg);
    out << "Best R = "  << bestR2 << endl;


    uint32_t bestL3 = 0;
    cout << "\nSearching L3...\n";
    attackL3(L1, L2, L3, z, bestL1, bestL2, L3_deg, bestL3);
    out << "\n===== ATTACK ON L3 =====\n";
    out << "\nRecovered L3:\n";

    printBits(out, bestL3, L3_deg);

    Geffe geffe(L1, L2, L3);
    int err = verifyKey(geffe, z, bestL1, bestL2,bestL3);

    out << "\n===== KEY VERIFICATION =====\n";

    out << "Errors = "<< err<< endl;

    if (err == 0)
        out << "SUCCESS\n";
    else
        out << "FAILED\n";


    out << "\n===== FINAL RESULTS =====\n";

    out << "\nRecovered L1:\n";
    printBits(out, bestL1, L1_deg);

    out << "\nRecovered L2:\n";
    printBits(out, bestL2, L2_deg);

    out << "\nRecovered L3:\n";
    printBits(out, bestL3, L3_deg);

    out.close();
    candOut.close();

    return 0;
}