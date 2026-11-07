#ifndef ENIGMA_PLUGBOARD_H
#define ENIGMA_PLUGBOARD_H

#include "alphabet.h"

namespace enigma {

class Plugboard {
public:
    Plugboard();
    void reset();
    bool setPair(int a, int b);
    bool setPairLetters(char a, char b);
    bool parse(const char *spec);
    int map(int letter) const;
    void toString(char *out, int outSize) const;
    int pairCount() const { return pairCount_; }
    bool isIdentity() const { return pairCount_ == 0; }

private:
    uint8_t map_[ALPHABET_SIZE];
    int pairCount_;
};

}  // namespace enigma

#endif
