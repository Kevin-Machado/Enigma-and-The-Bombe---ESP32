#include "rotor.h"

namespace enigma {

// Wirings historicos documentados (Enigma I / M3, Wehrmacht / Luftwaffe).
// Fuentes: Tony Sale (codesandciphers.org.uk), Dirk Rijmenants,
// Wikipedia Enigma rotor details (tablas estandar).
// Notch: letra visible en la ventana cuando el rotor esta en muesca
// (el rotor de la izquierda avanza en el siguiente paso).

const RotorDef ROTOR_I = {"I", "EKMFLGDQVZNTOWYHXUSPAIBRCJ", "Q"};
const RotorDef ROTOR_II = {"II", "AJDKSIRUXBLHWTMCQGZNPYFVOE", "E"};
const RotorDef ROTOR_III = {"III", "BDFHJLCPRTXVZNYEIWGAKMUSQO", "V"};
const RotorDef ROTOR_IV = {"IV", "ESOVPZJAYQUIRHXLNFTGKDCMWB", "J"};
const RotorDef ROTOR_V = {"V", "VZBRGITYUPSDNHLXAWMJQOFECK", "Z"};
const RotorDef ROTOR_VI = {"VI", "JPGVOUMFYQBENHZRDKASXLICTW", "ZM"};
const RotorDef ROTOR_VII = {"VII", "NZJHGRCXMYSWBOUFAIVLPEKQDT", "ZM"};
const RotorDef ROTOR_VIII = {"VIII", "FKQHTLXOCBJSPDZRAMEWNIUYGV", "ZM"};

const RotorDef *const ROTOR_CATALOG[] = {
    &ROTOR_I, &ROTOR_II, &ROTOR_III, &ROTOR_IV,
    &ROTOR_V, &ROTOR_VI, &ROTOR_VII, &ROTOR_VIII
};
const int ROTOR_CATALOG_COUNT = 8;

static bool namesEqual(const char *a, const char *b) {
    if (a == nullptr || b == nullptr) {
        return false;
    }
    while (*a && *b) {
        char ca = *a;
        char cb = *b;
        if (ca >= 'a' && ca <= 'z') {
            ca = static_cast<char>(ca - 'a' + 'A');
        }
        if (cb >= 'a' && cb <= 'z') {
            cb = static_cast<char>(cb - 'a' + 'A');
        }
        if (ca != cb) {
            return false;
        }
        ++a;
        ++b;
    }
    return *a == *b;
}

const RotorDef *findRotor(const char *name) {
    for (int i = 0; i < ROTOR_CATALOG_COUNT; ++i) {
        if (namesEqual(ROTOR_CATALOG[i]->name, name)) {
            return ROTOR_CATALOG[i];
        }
    }
    return nullptr;
}

Rotor::Rotor()
    : name_(""),
      notchCount_(0),
      ring_(0),
      position_(0) {
    fillIdentity(fwd_);
    fillIdentity(inv_);
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        notches_[i] = 0;
    }
}

bool Rotor::configure(const RotorDef *def, int ring, int position) {
    if (def == nullptr) {
        return false;
    }
    if (!parseWiring(def->wiring, fwd_)) {
        return false;
    }
    invertPerm(fwd_, inv_);
    name_ = def->name;
    ring_ = mod26(ring);
    position_ = mod26(position);
    notchCount_ = 0;
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        notches_[i] = 0;
    }
    if (def->notches != nullptr) {
        for (int i = 0; def->notches[i] != '\0'; ++i) {
            int n = letterIndex(def->notches[i]);
            if (n < 0) {
                return false;
            }
            notches_[notchCount_++] = static_cast<uint8_t>(n);
        }
    }
    return true;
}

bool Rotor::configureByName(const char *name, int ring, int position) {
    const RotorDef *def = findRotor(name);
    if (def == nullptr) {
        return false;
    }
    return configure(def, ring, position);
}

void Rotor::setPosition(int position) {
    position_ = mod26(position);
}

int Rotor::forward(int letter) const {
    int shift = mod26(position_ - ring_);
    int contact = mod26(letter + shift);
    int wired = fwd_[contact];
    return mod26(wired - shift);
}

int Rotor::backward(int letter) const {
    int shift = mod26(position_ - ring_);
    int contact = mod26(letter + shift);
    int wired = inv_[contact];
    return mod26(wired - shift);
}

bool Rotor::atNotch() const {
    return hasNotchAt(position_);
}

bool Rotor::hasNotchAt(int pos) const {
    pos = mod26(pos);
    for (int i = 0; i < notchCount_; ++i) {
        if (notches_[i] == pos) {
            return true;
        }
    }
    return false;
}

void Rotor::step() {
    position_ = mod26(position_ + 1);
}

}  // namespace enigma
