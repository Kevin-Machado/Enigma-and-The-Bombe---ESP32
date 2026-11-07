#ifndef ENIGMA_MACHINE_H
#define ENIGMA_MACHINE_H

#include "rotor.h"
#include "plugboard.h"
#include "reflector.h"

namespace enigma {

struct MachineConfig {
    const char *rotorLeft;
    const char *rotorMiddle;
    const char *rotorRight;
    const char *reflector;
    char ringLeft;
    char ringMiddle;
    char ringRight;
    char posLeft;
    char posMiddle;
    char posRight;
    const char *plugboard;
};

class EnigmaMachine {
public:
    EnigmaMachine();

    bool configure(const MachineConfig &cfg);
    bool configure(
        const char *left,
        const char *middle,
        const char *right,
        const char *reflector,
        int ringL, int ringM, int ringR,
        int posL, int posM, int posR,
        const char *plugboard);

    void resetPositions(int posL, int posM, int posR);
    void setPositions(int posL, int posM, int posR);
    void getPositions(int &posL, int &posM, int &posR) const;
    void positionsToLetters(char *out3) const;

    void stepRotors();
    int scrambleCore(int letter) const;
    int encodeLetter(int letter);
    char encodeChar(char c);

    int processText(const char *in, char *out, int outSize, int *ignored);
    int processLettersOnly(const char *in, char *out, int outSize);

    const Rotor &left() const { return left_; }
    const Rotor &middle() const { return middle_; }
    const Rotor &right() const { return right_; }
    const Reflector &reflector() const { return reflector_; }
    const Plugboard &plugboard() const { return plugboard_; }
    bool ok() const { return ok_; }
    const char *error() const { return error_; }

    static bool letterToIndex(char c, int &out);
    static bool parsePos(char c, int &out);

private:
    Rotor left_;
    Rotor middle_;
    Rotor right_;
    Reflector reflector_;
    Plugboard plugboard_;
    int startL_;
    int startM_;
    int startR_;
    bool ok_;
    const char *error_;
    void setError(const char *msg);
};

bool configFromFields(
    MachineConfig &cfg,
    const char *rl, const char *rm, const char *rr, const char *ref,
    const char *rings, const char *pos, const char *plugs);

}  // namespace enigma

#endif
