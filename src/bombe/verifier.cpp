#include "verifier.h"

namespace bombe {

bool Verifier::verifyCrib(
    const enigma::MachineConfig &base,
    int posL, int posM, int posR,
    const CryptoMenu &menu,
    char *recovered,
    int recoveredSize) {
    enigma::EnigmaMachine m;
    enigma::MachineConfig cfg = base;
    cfg.posLeft = enigma::indexLetter(posL);
    cfg.posMiddle = enigma::indexLetter(posM);
    cfg.posRight = enigma::indexLetter(posR);
    if (!m.configure(cfg)) {
        return false;
    }
    const int skip = menu.alignment();
    for (int i = 0; i < skip; ++i) {
        m.stepRotors();
    }
    int n = menu.cribLength();
    if (recovered && recoveredSize > 0) {
        recovered[0] = '\0';
    }
    bool ok = true;
    for (int i = 0; i < n; ++i) {
        int c = enigma::letterIndex(menu.cipherSlice()[i]);
        char p = enigma::indexLetter(m.encodeLetter(c));
        if (recovered && i + 1 < recoveredSize) {
            recovered[i] = p;
            recovered[i + 1] = '\0';
        }
        if (p != menu.crib()[i]) {
            ok = false;
        }
    }
    return ok;
}

bool Verifier::verifyFullMessage(
    const enigma::MachineConfig &base,
    int posL, int posM, int posR,
    const char *ciphertext,
    char *out,
    int outSize) {
    enigma::EnigmaMachine m;
    enigma::MachineConfig cfg = base;
    cfg.posLeft = enigma::indexLetter(posL);
    cfg.posMiddle = enigma::indexLetter(posM);
    cfg.posRight = enigma::indexLetter(posR);
    if (!m.configure(cfg)) {
        return false;
    }
    m.processLettersOnly(ciphertext, out, outSize);
    return true;
}

}  // namespace bombe
