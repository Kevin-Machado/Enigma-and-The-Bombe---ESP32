#include "enigma_machine.h"

namespace enigma {

EnigmaMachine::EnigmaMachine()
    : startL_(0),
      startM_(0),
      startR_(0),
      ok_(false),
      error_("not configured") {}

void EnigmaMachine::setError(const char *msg) {
    ok_ = false;
    error_ = msg;
}

bool EnigmaMachine::letterToIndex(char c, int &out) {
    int v = letterIndex(c);
    if (v < 0) {
        return false;
    }
    out = v;
    return true;
}

bool EnigmaMachine::parsePos(char c, int &out) {
    return letterToIndex(c, out);
}

bool EnigmaMachine::configure(
    const char *left,
    const char *middle,
    const char *right,
    const char *reflector,
    int ringL, int ringM, int ringR,
    int posL, int posM, int posR,
    const char *plugboard) {
    ok_ = false;
    error_ = "";
    if (!left_ .configureByName(left, ringL, posL)) {
        setError("rotor izquierdo invalido");
        return false;
    }
    if (!middle_.configureByName(middle, ringM, posM)) {
        setError("rotor medio invalido");
        return false;
    }
    if (!right_.configureByName(right, ringR, posR)) {
        setError("rotor derecho invalido");
        return false;
    }
    if (findRotor(left) == findRotor(middle) ||
        findRotor(left) == findRotor(right) ||
        findRotor(middle) == findRotor(right)) {
        setError("los tres rotores deben ser distintos");
        return false;
    }
    if (!reflector_.configureByName(reflector)) {
        setError("reflector invalido");
        return false;
    }
    if (!plugboard_.parse(plugboard)) {
        setError("plugboard invalido");
        return false;
    }
    startL_ = mod26(posL);
    startM_ = mod26(posM);
    startR_ = mod26(posR);
    ok_ = true;
    error_ = "";
    return true;
}

bool EnigmaMachine::configure(const MachineConfig &cfg) {
    int rl = 0, rm = 0, rr = 0, pl = 0, pm = 0, pr = 0;
    if (!parsePos(cfg.ringLeft, rl) || !parsePos(cfg.ringMiddle, rm) || !parsePos(cfg.ringRight, rr)) {
        setError("ring settings invalidos");
        return false;
    }
    if (!parsePos(cfg.posLeft, pl) || !parsePos(cfg.posMiddle, pm) || !parsePos(cfg.posRight, pr)) {
        setError("posiciones iniciales invalidas");
        return false;
    }
    return configure(
        cfg.rotorLeft, cfg.rotorMiddle, cfg.rotorRight, cfg.reflector,
        rl, rm, rr, pl, pm, pr, cfg.plugboard);
}

void EnigmaMachine::resetPositions(int posL, int posM, int posR) {
    setPositions(posL, posM, posR);
    startL_ = mod26(posL);
    startM_ = mod26(posM);
    startR_ = mod26(posR);
}

void EnigmaMachine::setPositions(int posL, int posM, int posR) {
    left_.setPosition(posL);
    middle_.setPosition(posM);
    right_.setPosition(posR);
}

void EnigmaMachine::getPositions(int &posL, int &posM, int &posR) const {
    posL = left_.position();
    posM = middle_.position();
    posR = right_.position();
}

void EnigmaMachine::positionsToLetters(char *out3) const {
    out3[0] = indexLetter(left_.position());
    out3[1] = indexLetter(middle_.position());
    out3[2] = indexLetter(right_.position());
    out3[3] = '\0';
}

void EnigmaMachine::stepRotors() {
    // Mecanismo historico de doble paso (double stepping).
    // El rotor derecho avanza siempre.
    // Si el medio esta en muesca, avanza el medio y el izquierdo.
    // Si el derecho esta en muesca, avanza el medio.
    // Cuando el medio esta en muesca, el avance del medio ocurre dos veces
    // en pasos consecutivos: una por su propia muesca y otra por la del derecho
    // en el paso siguiente, de ahi el nombre "double stepping".
    bool middleNotch = middle_.atNotch();
    bool rightNotch = right_.atNotch();
    if (middleNotch) {
        left_.step();
        middle_.step();
    } else if (rightNotch) {
        middle_.step();
    }
    right_.step();
}

int EnigmaMachine::scrambleCore(int letter) const {
    // Recorrido: Plugboard -> R -> M -> L -> Reflector -> Linv -> Minv -> Rinv -> Plugboard
    int x = plugboard_.map(letter);
    x = right_.forward(x);
    x = middle_.forward(x);
    x = left_.forward(x);
    x = reflector_.map(x);
    x = left_.backward(x);
    x = middle_.backward(x);
    x = right_.backward(x);
    x = plugboard_.map(x);
    return x;
}

int EnigmaMachine::encodeLetter(int letter) {
    stepRotors();
    return scrambleCore(letter);
}

char EnigmaMachine::encodeChar(char c) {
    int idx = letterIndex(c);
    if (idx < 0) {
        return c;
    }
    return indexLetter(encodeLetter(idx));
}

int EnigmaMachine::processText(const char *in, char *out, int outSize, int *ignored) {
    if (outSize <= 0) {
        return 0;
    }
    int w = 0;
    int ign = 0;
    if (in == nullptr) {
        out[0] = '\0';
        if (ignored) {
            *ignored = 0;
        }
        return 0;
    }
    for (int i = 0; in[i] != '\0' && w + 1 < outSize; ++i) {
        char c = in[i];
        int idx = letterIndex(c);
        if (idx < 0) {
            if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
                continue;
            }
            ++ign;
            continue;
        }
        out[w++] = indexLetter(encodeLetter(idx));
    }
    out[w] = '\0';
    if (ignored) {
        *ignored = ign;
    }
    return w;
}

int EnigmaMachine::processLettersOnly(const char *in, char *out, int outSize) {
    return processText(in, out, outSize, nullptr);
}

bool configFromFields(
    MachineConfig &cfg,
    const char *rl, const char *rm, const char *rr, const char *ref,
    const char *rings, const char *pos, const char *plugs) {
    if (rl == nullptr || rm == nullptr || rr == nullptr || ref == nullptr) {
        return false;
    }
    if (rings == nullptr || pos == nullptr) {
        return false;
    }
    if (rings[0] == '\0' || rings[1] == '\0' || rings[2] == '\0') {
        return false;
    }
    if (pos[0] == '\0' || pos[1] == '\0' || pos[2] == '\0') {
        return false;
    }
    cfg.rotorLeft = rl;
    cfg.rotorMiddle = rm;
    cfg.rotorRight = rr;
    cfg.reflector = ref;
    cfg.ringLeft = toUpper(rings[0]);
    cfg.ringMiddle = toUpper(rings[1]);
    cfg.ringRight = toUpper(rings[2]);
    cfg.posLeft = toUpper(pos[0]);
    cfg.posMiddle = toUpper(pos[1]);
    cfg.posRight = toUpper(pos[2]);
    cfg.plugboard = (plugs == nullptr) ? "" : plugs;
    return true;
}

}  // namespace enigma
