#ifndef BOMBE_LEDS_H
#define BOMBE_LEDS_H

#include "../config/hardware_config.h"
#include <stdint.h>

namespace hardware {

class BombeLeds {
public:
    BombeLeds();
    void begin();
    void setYellow(bool on);
    void setGreen(bool on);
    void pulseRed();
    void tick();
    void allOff();
    bool yellow() const { return yellowOn_; }
    bool green() const { return greenOn_; }
    bool red() const { return redOn_; }
    static void setHostMillis(uint32_t ms);

private:
    void writePin(int pin, bool on);
    bool yellowOn_;
    bool greenOn_;
    bool redOn_;
    uint32_t redUntilMs_;
};

}  // namespace hardware

#endif
