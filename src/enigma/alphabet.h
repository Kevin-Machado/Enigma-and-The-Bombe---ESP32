#ifndef ENIGMA_ALPHABET_H
#define ENIGMA_ALPHABET_H

#include <stdint.h>
#include <stddef.h>

namespace enigma {

static const int ALPHABET_SIZE = 26;

inline int letterIndex(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a';
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    }
    return -1;
}

inline char indexLetter(int i) {
    return static_cast<char>('A' + ((i % ALPHABET_SIZE + ALPHABET_SIZE) % ALPHABET_SIZE));
}

inline int mod26(int x) {
    int r = x % ALPHABET_SIZE;
    if (r < 0) {
        r += ALPHABET_SIZE;
    }
    return r;
}

inline bool isLetter(char c) {
    return letterIndex(c) >= 0;
}

inline char toUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        return static_cast<char>(c - 'a' + 'A');
    }
    return c;
}

inline void fillIdentity(uint8_t *map) {
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        map[i] = static_cast<uint8_t>(i);
    }
}

inline void invertPerm(const uint8_t *fwd, uint8_t *inv) {
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        inv[fwd[i]] = static_cast<uint8_t>(i);
    }
}

inline bool parseWiring(const char *wiring, uint8_t *out) {
    if (wiring == nullptr) {
        return false;
    }
    bool seen[26] = {false};
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        int v = letterIndex(wiring[i]);
        if (v < 0 || seen[v]) {
            return false;
        }
        seen[v] = true;
        out[i] = static_cast<uint8_t>(v);
    }
    return wiring[ALPHABET_SIZE] == '\0';
}

}  // namespace enigma

#endif
