#ifndef ENIGMA_REFLECTOR_H
#define ENIGMA_REFLECTOR_H

#include "alphabet.h"

namespace enigma {

struct ReflectorDef {
    const char *name;
    const char *wiring;
};

extern const ReflectorDef REFLECTOR_A;
extern const ReflectorDef REFLECTOR_B;
extern const ReflectorDef REFLECTOR_C;
extern const ReflectorDef *const REFLECTOR_CATALOG[];
extern const int REFLECTOR_CATALOG_COUNT;

const ReflectorDef *findReflector(const char *name);

class Reflector {
public:
    Reflector();
    bool configure(const ReflectorDef *def);
    bool configureByName(const char *name);
    int map(int letter) const;
    const char *name() const { return name_; }

private:
    const char *name_;
    uint8_t map_[ALPHABET_SIZE];
};

}  // namespace enigma

#endif
