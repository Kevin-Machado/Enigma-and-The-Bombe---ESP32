#include "leds.h"

#if defined(ARDUINO) && !defined(HOST_BUILD)
#include <Arduino.h>
#else
static uint32_t hostMillis = 0;
uint32_t millis() {
    return hostMillis;
}
void pinMode(int, int) {}
void digitalWrite(int, int) {}
#ifndef OUTPUT
#define OUTPUT 1
#endif
#ifndef HIGH
#define HIGH 1
#define LOW 0
#endif
#endif

namespace hardware {

BombeLeds::BombeLeds()
    : yellowOn_(false),
      greenOn_(false),
      redOn_(false),
      redUntilMs_(0) {}

void BombeLeds::setHostMillis(uint32_t ms) {
#if !defined(ARDUINO) || defined(HOST_BUILD)
    hostMillis = ms;
#else
    (void)ms;
#endif
}

void BombeLeds::writePin(int pin, bool on) {
#if LED_ACTIVE_HIGH
    digitalWrite(pin, on ? HIGH : LOW);
#else
    digitalWrite(pin, on ? LOW : HIGH);
#endif
    (void)pin;
    (void)on;
}

void BombeLeds::begin() {
    pinMode(BOMBE_LED_YELLOW_PIN, OUTPUT);
    pinMode(BOMBE_LED_RED_PIN, OUTPUT);
    pinMode(BOMBE_LED_GREEN_PIN, OUTPUT);
    allOff();
}

void BombeLeds::setYellow(bool on) {
    yellowOn_ = on;
    writePin(BOMBE_LED_YELLOW_PIN, on);
}

void BombeLeds::setGreen(bool on) {
    greenOn_ = on;
    writePin(BOMBE_LED_GREEN_PIN, on);
}

void BombeLeds::pulseRed() {
    redOn_ = true;
    redUntilMs_ = millis() + RED_BLINK_MS;
    writePin(BOMBE_LED_RED_PIN, true);
}

void BombeLeds::tick() {
    if (redOn_ && millis() >= redUntilMs_) {
        redOn_ = false;
        writePin(BOMBE_LED_RED_PIN, false);
    }
}

void BombeLeds::allOff() {
    yellowOn_ = false;
    greenOn_ = false;
    redOn_ = false;
    redUntilMs_ = 0;
    writePin(BOMBE_LED_YELLOW_PIN, false);
    writePin(BOMBE_LED_RED_PIN, false);
    writePin(BOMBE_LED_GREEN_PIN, false);
}

}  // namespace hardware
