#ifndef BOMBE_VERIFIER_H
#define BOMBE_VERIFIER_H

#include "../enigma/enigma_machine.h"
#include "menu.h"

namespace bombe {

struct Candidate {
    int posL;
    int posM;
    int posR;
    int stecker[26];
    bool verified;
    char posLetters[4];
    char recovered[MAX_CRIB_LEN + 1];
};

class Verifier {
public:
    static bool verifyCrib(
        const enigma::MachineConfig &base,
        int posL, int posM, int posR,
        const CryptoMenu &menu,
        char *recovered,
        int recoveredSize);

    static bool verifyFullMessage(
        const enigma::MachineConfig &base,
        int posL, int posM, int posR,
        const char *ciphertext,
        char *out,
        int outSize);
};

}  // namespace bombe

#endif
