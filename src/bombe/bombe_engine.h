#ifndef BOMBE_ENGINE_H
#define BOMBE_ENGINE_H

#include "menu.h"
#include "constraints.h"
#include "verifier.h"
#include "../enigma/enigma_machine.h"

namespace bombe {

enum BombeState {
    BOMBE_IDLE = 0,
    BOMBE_RUNNING,
    BOMBE_PAUSED,
    BOMBE_CANCELLED,
    BOMBE_DONE,
    BOMBE_ERROR
};

struct BombeProgress {
    BombeState state;
    int evaluated;
    int discarded;
    int candidates;
    int verified;
    int stored;
    int total;
    int currentL;
    int currentM;
    int currentR;
    int lastDiscardL;
    int lastDiscardM;
    int lastDiscardR;
    DiscardReason lastReason;
    bool redPulse;
    bool greenOn;
    bool yellowOn;
    const char *error;
};

class BombeEngine {
public:
    BombeEngine();

    bool start(
        const char *ciphertext,
        const char *crib,
        int alignment,
        const char *rotorL,
        const char *rotorM,
        const char *rotorR,
        const char *reflector,
        const char *rings,
        const char *plugboard,
        const char **err);

    void cancel();
    void reset();
    bool stepSlice(int maxSteps);
    bool isRunning() const { return state_ == BOMBE_RUNNING; }
    bool isDone() const {
        return state_ == BOMBE_DONE || state_ == BOMBE_CANCELLED || state_ == BOMBE_ERROR;
    }

    void getProgress(BombeProgress &p) const;
    int candidateCount() const { return candCount_; }
    const Candidate &candidate(int i) const { return cands_[i]; }
    const CryptoMenu &menu() const { return menu_; }
    const char *ciphertext() const { return cipher_; }
    const char *stateName() const;
    const char *rotorLeft() const { return rotorLName_; }
    const char *rotorMiddle() const { return rotorMName_; }
    const char *rotorRight() const { return rotorRName_; }
    const char *reflector() const { return reflectorName_; }
    const char *rings() const { return rings_; }
    const char *plugs() const { return plugs_; }

    bool consumeRedPulse();
    bool yellowOn() const { return state_ == BOMBE_RUNNING; }
    bool greenOn() const { return verifiedCount_ > 0; }

private:
    void evaluateCurrent();
    void advancePosition();
    void captureCorePerm(uint8_t fwd[26]);
    void storeCandidate(const Candidate &c);

    BombeState state_;
    CryptoMenu menu_;
    ConstraintEngine constraints_;
    enigma::EnigmaMachine core_;
    enigma::MachineConfig baseCfg_;
    char rotorLName_[8];
    char rotorMName_[8];
    char rotorRName_[8];
    char reflectorName_[8];
    char rings_[4];
    char plugs_[64];
    char cipher_[MAX_MESSAGE_LEN + 1];
    char crib_[MAX_CRIB_LEN + 1];

    int posL_;
    int posM_;
    int posR_;
    int evaluated_;
    int discarded_;
    int candCount_;
    int candidatesFound_;
    int verifiedCount_;
    int total_;
    DiscardReason lastReason_;
    int lastDiscardL_;
    int lastDiscardM_;
    int lastDiscardR_;
    bool redPulse_;
    const char *error_;
    Candidate cands_[MAX_CANDIDATES];
};

}  // namespace bombe

#endif
