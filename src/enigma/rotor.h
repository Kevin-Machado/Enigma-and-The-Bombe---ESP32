#ifndef ENIGMA_ROTOR_H
#define ENIGMA_ROTOR_H

#include "alphabet.h"

namespace enigma {

struct RotorDef {
    const char *name;
    const char *wiring;
    const char *notches;
};

extern const RotorDef ROTOR_I;
extern const RotorDef ROTOR_II;
extern const RotorDef ROTOR_III;
extern const RotorDef ROTOR_IV;
extern const RotorDef ROTOR_V;
extern const RotorDef ROTOR_VI;
extern const RotorDef ROTOR_VII;
extern const RotorDef ROTOR_VIII;
extern const RotorDef *const ROTOR_CATALOG[];
extern const int ROTOR_CATALOG_COUNT;

const RotorDef *findRotor(const char *name);

class Rotor {
public:
    Rotor();
    bool configure(const RotorDef *def, int ring, int position);
    bool configureByName(const char *name, int ring, int position);

    void setPosition(int position);
    int position() const { return position_; }
    int ring() const { return ring_; }
    const char *name() const { return name_; }

    int forward(int letter) const;
    int backward(int letter) const;

    bool atNotch() const;
    void step();

    bool hasNotchAt(int pos) const;

private:
    const char *name_;
    uint8_t fwd_[ALPHABET_SIZE];
    uint8_t inv_[ALPHABET_SIZE];
    uint8_t notches_[ALPHABET_SIZE];
    int notchCount_;
    int ring_;
    int position_;
};

}  // namespace enigma

#endif
