#ifndef BOMBE_CONSTRAINTS_H
#define BOMBE_CONSTRAINTS_H

#include "menu.h"

namespace bombe {

enum DiscardReason {
    DISCARD_NONE = 0,
    DISCARD_STECKER_CONFLICT,
    DISCARD_INVOLUTION,
    DISCARD_NO_HYPOTHESIS,
    DISCARD_CORE_FIXED_POINT
};

const char *discardReasonText(DiscardReason r);

class ConstraintEngine {
public:
    ConstraintEngine();
    void setMenu(const CryptoMenu *menu);
    void setPermutation(int offset, const uint8_t fwd[26]);
    void clearPerms();
    void setKnownStecker(const int *known);

    DiscardReason analyze(int *steckerOut, int *hypothesesOk);

    static bool assignStecker(int *s, int a, int b);
    static DiscardReason propagate(const CryptoMenu *menu, const uint8_t (*fwd)[26], int *s);

private:
    const CryptoMenu *menu_;
    uint8_t fwd_[MAX_CRIB_LEN][26];
    bool havePerm_[MAX_CRIB_LEN];
    int known_[26];
};

}  // namespace bombe

#endif
