#include "constraints.h"

namespace bombe {

const char *discardReasonText(DiscardReason r) {
    switch (r) {
        case DISCARD_NONE:
            return "sin descarte";
        case DISCARD_STECKER_CONFLICT:
            return "contradiccion de stecker: una letra exigiria dos imagenes distintas";
        case DISCARD_INVOLUTION:
            return "contradiccion: el stecker dejaria de ser una involucion";
        case DISCARD_NO_HYPOTHESIS:
            return "las hipotesis del registro de prueba producen contradiccion";
        case DISCARD_CORE_FIXED_POINT:
            return "el nucleo cifraria una letra como si misma";
        default:
            return "contradiccion logica";
    }
}

ConstraintEngine::ConstraintEngine() : menu_(nullptr) {
    clearPerms();
    for (int i = 0; i < 26; ++i) {
        known_[i] = -1;
    }
}

void ConstraintEngine::setMenu(const CryptoMenu *menu) {
    menu_ = menu;
}

void ConstraintEngine::clearPerms() {
    for (int t = 0; t < MAX_CRIB_LEN; ++t) {
        havePerm_[t] = false;
        enigma::fillIdentity(fwd_[t]);
    }
}

void ConstraintEngine::setKnownStecker(const int *known) {
    for (int i = 0; i < 26; ++i) {
        known_[i] = known ? known[i] : -1;
    }
}

void ConstraintEngine::setPermutation(int offset, const uint8_t fwd[26]) {
    if (offset < 0 || offset >= MAX_CRIB_LEN) {
        return;
    }
    for (int i = 0; i < 26; ++i) {
        fwd_[offset][i] = fwd[i];
    }
    havePerm_[offset] = true;
}

bool ConstraintEngine::assignStecker(int *s, int a, int b) {
    if (a < 0 || a >= 26 || b < 0 || b >= 26) {
        return false;
    }
    if (s[a] == b && s[b] == a) {
        return true;
    }
    if (s[a] >= 0 && s[a] != b) {
        return false;
    }
    if (s[b] >= 0 && s[b] != a) {
        return false;
    }
    s[a] = b;
    s[b] = a;
    return true;
}

DiscardReason ConstraintEngine::propagate(const CryptoMenu *menu, const uint8_t (*fwd)[26], int *s) {
    if (menu == nullptr) {
        return DISCARD_NONE;
    }
    bool changed = true;
    int guard = 0;
    while (changed && guard < 64) {
        changed = false;
        ++guard;
        for (int i = 0; i < menu->linkCount(); ++i) {
            const MenuLink &e = menu->link(i);
            int p = e.plain;
            int c = e.cipher;
            int t = e.offset;
            int sp = s[p];
            int sc = s[c];
            if (sp >= 0) {
                int implied = fwd[t][sp];
                if (implied == sp) {
                    return DISCARD_CORE_FIXED_POINT;
                }
                if (sc >= 0 && sc != implied) {
                    return DISCARD_STECKER_CONFLICT;
                }
                if (sc < 0) {
                    if (!assignStecker(s, c, implied)) {
                        return DISCARD_INVOLUTION;
                    }
                    changed = true;
                }
            }
            if (sc >= 0) {
                int impliedP = -1;
                for (int x = 0; x < 26; ++x) {
                    if (fwd[t][x] == sc) {
                        impliedP = x;
                        break;
                    }
                }
                if (impliedP < 0) {
                    return DISCARD_STECKER_CONFLICT;
                }
                if (sp >= 0 && sp != impliedP) {
                    return DISCARD_STECKER_CONFLICT;
                }
                if (sp < 0) {
                    if (!assignStecker(s, p, impliedP)) {
                        return DISCARD_INVOLUTION;
                    }
                    changed = true;
                }
            }
        }
    }
    return DISCARD_NONE;
}

DiscardReason ConstraintEngine::analyze(int *steckerOut, int *hypothesesOk) {
    if (menu_ == nullptr) {
        return DISCARD_NO_HYPOTHESIS;
    }
    int testLetter = menu_->mostFrequentLetter();
    int okCount = 0;
    int bestS[26];
    int bestKnown = -1;
    for (int i = 0; i < 26; ++i) {
        bestS[i] = -1;
    }

    int hypStart = 0;
    int hypEnd = 26;
    if (known_[testLetter] >= 0) {
        hypStart = known_[testLetter];
        hypEnd = hypStart + 1;
    }

    for (int hyp = hypStart; hyp < hypEnd; ++hyp) {
        int s[26];
        for (int i = 0; i < 26; ++i) {
            s[i] = -1;
        }
        bool preOk = true;
        for (int i = 0; i < 26; ++i) {
            if (known_[i] >= 0) {
                if (!assignStecker(s, i, known_[i])) {
                    preOk = false;
                    break;
                }
            }
        }
        if (!preOk) {
            continue;
        }
        if (!assignStecker(s, testLetter, hyp)) {
            continue;
        }
        DiscardReason r = propagate(menu_, fwd_, s);
        if (r != DISCARD_NONE) {
            continue;
        }
        okCount++;
        int knownCount = 0;
        for (int i = 0; i < 26; ++i) {
            if (s[i] >= 0) {
                ++knownCount;
            }
        }
        if (knownCount > bestKnown) {
            bestKnown = knownCount;
            for (int i = 0; i < 26; ++i) {
                bestS[i] = s[i];
            }
        }
    }
    if (hypothesesOk) {
        *hypothesesOk = okCount;
    }
    if (okCount == 0) {
        return DISCARD_NO_HYPOTHESIS;
    }
    if (steckerOut) {
        for (int i = 0; i < 26; ++i) {
            steckerOut[i] = bestS[i];
        }
    }
    return DISCARD_NONE;
}

}  // namespace bombe
