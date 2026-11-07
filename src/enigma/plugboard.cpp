#include "plugboard.h"

namespace enigma {

Plugboard::Plugboard() {
    reset();
}

void Plugboard::reset() {
    fillIdentity(map_);
    pairCount_ = 0;
}

bool Plugboard::setPair(int a, int b) {
    if (a < 0 || a >= ALPHABET_SIZE || b < 0 || b >= ALPHABET_SIZE) {
        return false;
    }
    if (a == b) {
        return false;
    }
    if (map_[a] != a || map_[b] != b) {
        return false;
    }
    map_[a] = static_cast<uint8_t>(b);
    map_[b] = static_cast<uint8_t>(a);
    pairCount_++;
    return true;
}

bool Plugboard::setPairLetters(char a, char b) {
    return setPair(letterIndex(a), letterIndex(b));
}

bool Plugboard::parse(const char *spec) {
    reset();
    if (spec == nullptr || spec[0] == '\0') {
        return true;
    }
    int i = 0;
    while (spec[i] != '\0') {
        while (spec[i] == ' ' || spec[i] == ',' || spec[i] == ';' || spec[i] == '|') {
            ++i;
        }
        if (spec[i] == '\0') {
            break;
        }
        int a = letterIndex(spec[i]);
        if (a < 0) {
            reset();
            return false;
        }
        ++i;
        if (spec[i] == '-' || spec[i] == ':') {
            ++i;
        }
        int b = letterIndex(spec[i]);
        if (b < 0) {
            reset();
            return false;
        }
        ++i;
        if (!setPair(a, b)) {
            reset();
            return false;
        }
    }
    return true;
}

int Plugboard::map(int letter) const {
    if (letter < 0 || letter >= ALPHABET_SIZE) {
        return letter;
    }
    return map_[letter];
}

void Plugboard::toString(char *out, int outSize) const {
    if (out == nullptr || outSize <= 0) {
        return;
    }
    int n = 0;
    bool used[26] = {false};
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (used[i]) {
            continue;
        }
        int j = map_[i];
        if (j == i) {
            continue;
        }
        if (n + 3 >= outSize) {
            break;
        }
        if (n > 0) {
            out[n++] = ' ';
        }
        out[n++] = indexLetter(i);
        out[n++] = indexLetter(j);
        used[i] = true;
        used[j] = true;
    }
    out[n] = '\0';
}

}  // namespace enigma
