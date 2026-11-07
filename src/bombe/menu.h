#ifndef BOMBE_MENU_H
#define BOMBE_MENU_H

#include "../enigma/alphabet.h"
#include "../config/hardware_config.h"

namespace bombe {

struct MenuLink {
    int plain;
    int cipher;
    int offset;
};

class CryptoMenu {
public:
    CryptoMenu();
    void clear();
    bool build(const char *ciphertext, const char *crib, int alignment, const char **err);
    int linkCount() const { return count_; }
    const MenuLink &link(int i) const { return links_[i]; }
    int cribLength() const { return cribLen_; }
    int alignment() const { return align_; }
    const char *crib() const { return crib_; }
    const char *cipherSlice() const { return slice_; }
    int letterFreq(int letter) const { return freq_[letter]; }
    int mostFrequentLetter() const;
    bool hasSelfEncryption() const { return selfEncrypt_; }
    int selfEncryptIndex() const { return selfIdx_; }

private:
    MenuLink links_[MAX_MENU_LINKS];
    int count_;
    int cribLen_;
    int align_;
    bool selfEncrypt_;
    int selfIdx_;
    int freq_[enigma::ALPHABET_SIZE];
    char crib_[MAX_CRIB_LEN + 1];
    char slice_[MAX_CRIB_LEN + 1];
};

}  // namespace bombe

#endif
