#include "reflector.h"

namespace enigma {

// Reflectores historicos Enigma I / M3.
// UKW-A, UKW-B, UKW-C. Fuente: Tony Sale / Rijmenants.
const ReflectorDef REFLECTOR_A = {"A", "EJMZALYXVBWFCRQUONTSPIKHGD"};
const ReflectorDef REFLECTOR_B = {"B", "YRUHQSLDPXNGOKMIEBFZCWVJAT"};
const ReflectorDef REFLECTOR_C = {"C", "FVPJIAOYEDRZXWGCTKUQSBNMHL"};

const ReflectorDef *const REFLECTOR_CATALOG[] = {
    &REFLECTOR_A, &REFLECTOR_B, &REFLECTOR_C
};
const int REFLECTOR_CATALOG_COUNT = 3;

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

const ReflectorDef *findReflector(const char *name) {
    if (name == nullptr) {
        return nullptr;
    }
    if (namesEqual(name, "UKW-A") || namesEqual(name, "UKWA") || namesEqual(name, "A")) {
        return &REFLECTOR_A;
    }
    if (namesEqual(name, "UKW-B") || namesEqual(name, "UKWB") || namesEqual(name, "B")) {
        return &REFLECTOR_B;
    }
    if (namesEqual(name, "UKW-C") || namesEqual(name, "UKWC") || namesEqual(name, "C")) {
        return &REFLECTOR_C;
    }
    for (int i = 0; i < REFLECTOR_CATALOG_COUNT; ++i) {
        if (namesEqual(REFLECTOR_CATALOG[i]->name, name)) {
            return REFLECTOR_CATALOG[i];
        }
    }
    return nullptr;
}

Reflector::Reflector() : name_("") {
    fillIdentity(map_);
}

bool Reflector::configure(const ReflectorDef *def) {
    if (def == nullptr) {
        return false;
    }
    uint8_t tmp[ALPHABET_SIZE];
    if (!parseWiring(def->wiring, tmp)) {
        return false;
    }
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        if (tmp[i] == i) {
            return false;
        }
        if (tmp[tmp[i]] != i) {
            return false;
        }
    }
    for (int i = 0; i < ALPHABET_SIZE; ++i) {
        map_[i] = tmp[i];
    }
    name_ = def->name;
    return true;
}

bool Reflector::configureByName(const char *name) {
    const ReflectorDef *def = findReflector(name);
    if (def == nullptr) {
        return false;
    }
    return configure(def);
}

int Reflector::map(int letter) const {
    if (letter < 0 || letter >= ALPHABET_SIZE) {
        return letter;
    }
    return map_[letter];
}

}  // namespace enigma
