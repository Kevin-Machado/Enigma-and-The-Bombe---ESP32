#include "menu.h"

namespace bombe {

CryptoMenu::CryptoMenu() {
    clear();
}

void CryptoMenu::clear() {
    count_ = 0;
    cribLen_ = 0;
    align_ = 0;
    selfEncrypt_ = false;
    selfIdx_ = -1;
    crib_[0] = '\0';
    slice_[0] = '\0';
    for (int i = 0; i < enigma::ALPHABET_SIZE; ++i) {
        freq_[i] = 0;
    }
}

bool CryptoMenu::build(const char *ciphertext, const char *crib, int alignment, const char **err) {
    clear();
    if (ciphertext == nullptr || crib == nullptr) {
        if (err) {
            *err = "texto cifrado o crib vacio";
        }
        return false;
    }
    char cipherBuf[MAX_MESSAGE_LEN + 1];
    char cribBuf[MAX_CRIB_LEN + 1];
    int clen = 0;
    int klen = 0;
    for (int i = 0; ciphertext[i] != '\0' && clen < MAX_MESSAGE_LEN; ++i) {
        int idx = enigma::letterIndex(ciphertext[i]);
        if (idx < 0) {
            continue;
        }
        cipherBuf[clen++] = enigma::indexLetter(idx);
    }
    cipherBuf[clen] = '\0';
    for (int i = 0; crib[i] != '\0' && klen < MAX_CRIB_LEN; ++i) {
        int idx = enigma::letterIndex(crib[i]);
        if (idx < 0) {
            continue;
        }
        cribBuf[klen++] = enigma::indexLetter(idx);
    }
    cribBuf[klen] = '\0';
    if (clen == 0) {
        if (err) {
            *err = "el texto cifrado no contiene letras";
        }
        return false;
    }
    if (klen < 3) {
        if (err) {
            *err = "el crib debe tener al menos 3 letras";
        }
        return false;
    }
    if (alignment < 0 || alignment + klen > clen) {
        if (err) {
            *err = "alineacion del crib fuera de rango";
        }
        return false;
    }
    if (klen > MAX_MENU_LINKS) {
        if (err) {
            *err = "crib demasiado largo";
        }
        return false;
    }
    cribLen_ = klen;
    align_ = alignment;
    for (int i = 0; i <= klen; ++i) {
        crib_[i] = cribBuf[i];
        slice_[i] = (i < klen) ? cipherBuf[alignment + i] : '\0';
    }
    for (int i = 0; i < klen; ++i) {
        int p = enigma::letterIndex(crib_[i]);
        int c = enigma::letterIndex(slice_[i]);
        if (p == c) {
            selfEncrypt_ = true;
            selfIdx_ = i;
            if (err) {
                *err = "el crib coincide con el cifrado en una letra (Enigma nunca cifra una letra como si misma)";
            }
            return false;
        }
        links_[count_].plain = p;
        links_[count_].cipher = c;
        links_[count_].offset = i;
        count_++;
        freq_[p]++;
        freq_[c]++;
    }
    if (err) {
        *err = "";
    }
    return true;
}

int CryptoMenu::mostFrequentLetter() const {
    int best = 0;
    int bestF = freq_[0];
    for (int i = 1; i < enigma::ALPHABET_SIZE; ++i) {
        if (freq_[i] > bestF) {
            bestF = freq_[i];
            best = i;
        }
    }
    return best;
}

}  // namespace bombe
