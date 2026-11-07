#include "bombe_engine.h"

namespace bombe {

static void copyStr(char *dst, int dstSize, const char *src) {
    if (dstSize <= 0) {
        return;
    }
    int i = 0;
    if (src) {
        while (src[i] != '\0' && i + 1 < dstSize) {
            dst[i] = src[i];
            ++i;
        }
    }
    dst[i] = '\0';
}

BombeEngine::BombeEngine() {
    reset();
}

void BombeEngine::reset() {
    state_ = BOMBE_IDLE;
    menu_.clear();
    constraints_.clearPerms();
    rotorLName_[0] = '\0';
    rotorMName_[0] = '\0';
    rotorRName_[0] = '\0';
    reflectorName_[0] = '\0';
    rings_[0] = 'A';
    rings_[1] = 'A';
    rings_[2] = 'A';
    rings_[3] = '\0';
    plugs_[0] = '\0';
    cipher_[0] = '\0';
    crib_[0] = '\0';
    posL_ = 0;
    posM_ = 0;
    posR_ = 0;
    evaluated_ = 0;
    discarded_ = 0;
    candCount_ = 0;
    candidatesFound_ = 0;
    verifiedCount_ = 0;
    total_ = 26 * 26 * 26;
    lastReason_ = DISCARD_NONE;
    lastDiscardL_ = 0;
    lastDiscardM_ = 0;
    lastDiscardR_ = 0;
    redPulse_ = false;
    error_ = "";
}

bool BombeEngine::start(
    const char *ciphertext,
    const char *crib,
    int alignment,
    const char *rotorL,
    const char *rotorM,
    const char *rotorR,
    const char *reflector,
    const char *rings,
    const char *plugboard,
    const char **err) {
    reset();
    copyStr(cipher_, sizeof(cipher_), ciphertext);
    copyStr(crib_, sizeof(crib_), crib);
    copyStr(rotorLName_, sizeof(rotorLName_), rotorL);
    copyStr(rotorMName_, sizeof(rotorMName_), rotorM);
    copyStr(rotorRName_, sizeof(rotorRName_), rotorR);
    copyStr(reflectorName_, sizeof(reflectorName_), reflector);
    if (rings && rings[0] && rings[1] && rings[2]) {
        rings_[0] = enigma::toUpper(rings[0]);
        rings_[1] = enigma::toUpper(rings[1]);
        rings_[2] = enigma::toUpper(rings[2]);
        rings_[3] = '\0';
    }
    copyStr(plugs_, sizeof(plugs_), plugboard);

    const char *menuErr = "";
    if (!menu_.build(cipher_, crib_, alignment, &menuErr)) {
        state_ = BOMBE_ERROR;
        error_ = menuErr;
        if (err) {
            *err = menuErr;
        }
        return false;
    }

    baseCfg_.rotorLeft = rotorLName_;
    baseCfg_.rotorMiddle = rotorMName_;
    baseCfg_.rotorRight = rotorRName_;
    baseCfg_.reflector = reflectorName_;
    baseCfg_.ringLeft = rings_[0];
    baseCfg_.ringMiddle = rings_[1];
    baseCfg_.ringRight = rings_[2];
    baseCfg_.posLeft = 'A';
    baseCfg_.posMiddle = 'A';
    baseCfg_.posRight = 'A';
    baseCfg_.plugboard = plugs_;

    if (!core_.configure(baseCfg_)) {
        state_ = BOMBE_ERROR;
        error_ = core_.error();
        if (err) {
            *err = error_;
        }
        return false;
    }

    int known[26];
    for (int i = 0; i < 26; ++i) {
        known[i] = core_.plugboard().map(i);
    }
    constraints_.setMenu(&menu_);
    constraints_.setKnownStecker(known);

    posL_ = 0;
    posM_ = 0;
    posR_ = 0;
    state_ = BOMBE_RUNNING;
    error_ = "";
    if (err) {
        *err = "";
    }
    return true;
}

void BombeEngine::cancel() {
    if (state_ == BOMBE_RUNNING) {
        state_ = BOMBE_CANCELLED;
    }
}

void BombeEngine::captureCorePerm(uint8_t fwd[26]) {
    for (int in = 0; in < 26; ++in) {
        int x = core_.right().forward(in);
        x = core_.middle().forward(x);
        x = core_.left().forward(x);
        x = core_.reflector().map(x);
        x = core_.left().backward(x);
        x = core_.middle().backward(x);
        x = core_.right().backward(x);
        fwd[in] = static_cast<uint8_t>(x);
    }
}

void BombeEngine::storeCandidate(const Candidate &c) {
    if (candCount_ < MAX_CANDIDATES) {
        cands_[candCount_++] = c;
        return;
    }
    if (!c.verified) {
        return;
    }
    for (int i = 0; i < candCount_; ++i) {
        if (!cands_[i].verified) {
            cands_[i] = c;
            return;
        }
    }
}

void BombeEngine::evaluateCurrent() {
    core_.setPositions(posL_, posM_, posR_);
    constraints_.clearPerms();
    uint8_t fwd[26];
    const int startAt = menu_.alignment();
    const int endAt = startAt + menu_.cribLength();
    for (int k = 0; k < endAt; ++k) {
        core_.stepRotors();
        if (k >= startAt) {
            captureCorePerm(fwd);
            constraints_.setPermutation(k - startAt, fwd);
        }
    }
    int stecker[26];
    int hypOk = 0;
    DiscardReason reason = constraints_.analyze(stecker, &hypOk);
    evaluated_++;
    if (reason != DISCARD_NONE) {
        discarded_++;
        lastReason_ = reason;
        lastDiscardL_ = posL_;
        lastDiscardM_ = posM_;
        lastDiscardR_ = posR_;
        redPulse_ = true;
        return;
    }
    candidatesFound_++;
    Candidate c;
    c.posL = posL_;
    c.posM = posM_;
    c.posR = posR_;
    for (int i = 0; i < 26; ++i) {
        c.stecker[i] = stecker[i];
    }
    c.posLetters[0] = enigma::indexLetter(posL_);
    c.posLetters[1] = enigma::indexLetter(posM_);
    c.posLetters[2] = enigma::indexLetter(posR_);
    c.posLetters[3] = '\0';
    c.recovered[0] = '\0';
    c.verified = Verifier::verifyCrib(
        baseCfg_, posL_, posM_, posR_, menu_, c.recovered, sizeof(c.recovered));
    if (c.verified) {
        verifiedCount_++;
    }
    storeCandidate(c);
}

void BombeEngine::advancePosition() {
    posR_++;
    if (posR_ >= 26) {
        posR_ = 0;
        posM_++;
        if (posM_ >= 26) {
            posM_ = 0;
            posL_++;
        }
    }
}

bool BombeEngine::stepSlice(int maxSteps) {
    if (state_ != BOMBE_RUNNING) {
        return false;
    }
    if (maxSteps < 1) {
        maxSteps = 1;
    }
    for (int i = 0; i < maxSteps; ++i) {
        if (posL_ >= 26) {
            state_ = BOMBE_DONE;
            return false;
        }
        evaluateCurrent();
        advancePosition();
        if (posL_ >= 26) {
            state_ = BOMBE_DONE;
            return false;
        }
    }
    return true;
}

void BombeEngine::getProgress(BombeProgress &p) const {
    p.state = state_;
    p.evaluated = evaluated_;
    p.discarded = discarded_;
    p.candidates = candidatesFound_;
    p.verified = verifiedCount_;
    p.stored = candCount_;
    p.total = total_;
    p.currentL = posL_;
    p.currentM = posM_;
    p.currentR = posR_;
    p.lastDiscardL = lastDiscardL_;
    p.lastDiscardM = lastDiscardM_;
    p.lastDiscardR = lastDiscardR_;
    p.lastReason = lastReason_;
    p.redPulse = redPulse_;
    p.greenOn = verifiedCount_ > 0;
    p.yellowOn = state_ == BOMBE_RUNNING;
    p.error = error_;
}

bool BombeEngine::consumeRedPulse() {
    bool v = redPulse_;
    redPulse_ = false;
    return v;
}

const char *BombeEngine::stateName() const {
    switch (state_) {
        case BOMBE_IDLE:
            return "idle";
        case BOMBE_RUNNING:
            return "running";
        case BOMBE_PAUSED:
            return "paused";
        case BOMBE_CANCELLED:
            return "cancelled";
        case BOMBE_DONE:
            return "done";
        case BOMBE_ERROR:
            return "error";
        default:
            return "unknown";
    }
}

}  // namespace bombe
