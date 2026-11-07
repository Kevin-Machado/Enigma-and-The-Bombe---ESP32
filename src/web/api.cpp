#include "api.h"
#include "json_util.h"
#include "../enigma/enigma_machine.h"
#include "../bombe/bombe_engine.h"
#include "../hardware/leds.h"
#include "../config/hardware_config.h"

namespace web {

static enigma::EnigmaMachine gEnigma;
static bombe::BombeEngine gBombe;
static hardware::BombeLeds gLeds;
static char gLastEnigmaOut[MAX_MESSAGE_LEN + 1];
static char gLastEnigmaIn[MAX_MESSAGE_LEN + 1];
static bool gEnigmaConfigured = false;

static void copyStr(char *dst, int dstSize, const char *src) {
    int i = 0;
    if (src) {
        while (src[i] && i + 1 < dstSize) {
            dst[i] = src[i];
            ++i;
        }
    }
    dst[i] = '\0';
}

static void jsonOkPrefix(char *out, int outSize, int &n) {
    n = 0;
    out[0] = '\0';
    append(out, outSize, n, "{\"ok\":true");
}

static void jsonErr(char *out, int outSize, const char *msg) {
    int n = 0;
    out[0] = '\0';
    append(out, outSize, n, "{\"ok\":false,\"error\":\"");
    char esc[128];
    jsonEscape(msg ? msg : "error", esc, sizeof(esc));
    append(out, outSize, n, esc);
    append(out, outSize, n, "\"}");
}

static void appendPositions(char *out, int outSize, int &n, const enigma::EnigmaMachine &m) {
    char pos[4];
    m.positionsToLetters(pos);
    append(out, outSize, n, "\"positions\":\"");
    append(out, outSize, n, pos);
    append(out, outSize, n, "\"");
}

void apiInit() {
    gEnigmaConfigured = false;
    gLastEnigmaOut[0] = '\0';
    gLastEnigmaIn[0] = '\0';
    gBombe.reset();
    gLeds.begin();
    gLeds.allOff();
}

void apiTick() {
    if (gBombe.isRunning()) {
        gBombe.stepSlice(BOMBE_STEPS_PER_SLICE);
        gLeds.setYellow(true);
        if (gBombe.consumeRedPulse()) {
            gLeds.pulseRed();
        }
        if (gBombe.greenOn()) {
            gLeds.setGreen(true);
        }
    } else {
        gLeds.setYellow(false);
        if (gBombe.greenOn()) {
            gLeds.setGreen(true);
        }
    }
    gLeds.tick();
}

static bool parseEnigmaConfig(const char *body, enigma::MachineConfig &cfg, char names[4][8], char rings[8], char pos[8], char plugs[64], const char **err) {
    names[0][0] = names[1][0] = names[2][0] = names[3][0] = '\0';
    rings[0] = '\0';
    pos[0] = '\0';
    plugs[0] = '\0';
    bool haveRotors = jsonGetString(body, "rotorLeft", names[0], 8) &&
                      jsonGetString(body, "rotorMiddle", names[1], 8) &&
                      jsonGetString(body, "rotorRight", names[2], 8);
    if (!haveRotors) {
        haveRotors = jsonGetStringArray3(body, "rotors", names);
    }
    if (!haveRotors) {
        *err = "faltan nombres de rotores";
        return false;
    }
    if (!jsonGetString(body, "reflector", names[3], 8)) {
        *err = "falta reflector";
        return false;
    }
    if (!jsonGetString(body, "rings", rings, 8)) {
        *err = "faltan ring settings";
        return false;
    }
    if (!jsonGetString(body, "positions", pos, 8)) {
        *err = "faltan posiciones";
        return false;
    }
    jsonGetString(body, "plugboard", plugs, 64);
    if (!enigma::configFromFields(cfg, names[0], names[1], names[2], names[3], rings, pos, plugs)) {
        *err = "configuracion incompleta";
        return false;
    }
    return true;
}

void apiEnigma(const char *body, char *out, int outSize) {
    char names[4][8];
    char rings[8];
    char pos[8];
    char plugs[64];
    char text[MAX_MESSAGE_LEN + 1];
    enigma::MachineConfig cfg;
    const char *err = "";
    if (!parseEnigmaConfig(body, cfg, names, rings, pos, plugs, &err)) {
        jsonErr(out, outSize, err);
        return;
    }
    if (!jsonGetString(body, "text", text, sizeof(text))) {
        jsonErr(out, outSize, "falta texto");
        return;
    }
    if (!gEnigma.configure(cfg)) {
        jsonErr(out, outSize, gEnigma.error());
        return;
    }
    gEnigmaConfigured = true;
    copyStr(gLastEnigmaIn, sizeof(gLastEnigmaIn), text);
    int ignored = 0;
    gEnigma.processText(text, gLastEnigmaOut, sizeof(gLastEnigmaOut), &ignored);
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"result\":\"");
    char esc[MAX_MESSAGE_LEN * 2];
    jsonEscape(gLastEnigmaOut, esc, sizeof(esc));
    append(out, outSize, n, esc);
    append(out, outSize, n, "\",\"ignored\":");
    appendInt(out, outSize, n, ignored);
    append(out, outSize, n, ",");
    appendPositions(out, outSize, n, gEnigma);
    append(out, outSize, n, ",\"rotors\":[\"");
    append(out, outSize, n, gEnigma.left().name());
    append(out, outSize, n, "\",\"");
    append(out, outSize, n, gEnigma.middle().name());
    append(out, outSize, n, "\",\"");
    append(out, outSize, n, gEnigma.right().name());
    append(out, outSize, n, "\"],\"reflector\":\"");
    append(out, outSize, n, gEnigma.reflector().name());
    append(out, outSize, n, "\"}");
}

void apiEnigmaReset(const char *body, char *out, int outSize) {
    char names[4][8];
    char rings[8];
    char pos[8];
    char plugs[64];
    enigma::MachineConfig cfg;
    const char *err = "";
    if (body && body[0] && parseEnigmaConfig(body, cfg, names, rings, pos, plugs, &err)) {
        if (!gEnigma.configure(cfg)) {
            jsonErr(out, outSize, gEnigma.error());
            return;
        }
        gEnigmaConfigured = true;
    } else {
        gEnigmaConfigured = false;
    }
    gLastEnigmaOut[0] = '\0';
    gLastEnigmaIn[0] = '\0';
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"reset\":true}");
}

void apiEnigmaState(char *out, int outSize) {
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"configured\":");
    append(out, outSize, n, gEnigmaConfigured ? "true" : "false");
    if (gEnigmaConfigured) {
        append(out, outSize, n, ",");
        appendPositions(out, outSize, n, gEnigma);
    }
    append(out, outSize, n, "}");
}

void apiBombeStart(const char *body, char *out, int outSize) {
    char cipher[MAX_MESSAGE_LEN + 1];
    char crib[MAX_CRIB_LEN + 1];
    char names[4][8];
    char rings[8];
    char plugs[64];
    int alignment = 0;
    cipher[0] = crib[0] = rings[0] = plugs[0] = '\0';
    names[0][0] = names[1][0] = names[2][0] = names[3][0] = '\0';
    if (!jsonGetString(body, "ciphertext", cipher, sizeof(cipher)) &&
        !jsonGetString(body, "text", cipher, sizeof(cipher))) {
        jsonErr(out, outSize, "falta texto cifrado");
        return;
    }
    if (!jsonGetString(body, "crib", crib, sizeof(crib))) {
        jsonErr(out, outSize, "falta crib");
        return;
    }
    jsonGetInt(body, "alignment", &alignment);
    bool haveRotors = jsonGetString(body, "rotorLeft", names[0], 8) &&
                      jsonGetString(body, "rotorMiddle", names[1], 8) &&
                      jsonGetString(body, "rotorRight", names[2], 8);
    if (!haveRotors) {
        haveRotors = jsonGetStringArray3(body, "rotors", names);
    }
    if (!haveRotors) {
        jsonErr(out, outSize, "faltan rotores");
        return;
    }
    if (names[0][0] == '\0' || names[1][0] == '\0' || names[2][0] == '\0') {
        jsonErr(out, outSize, "faltan rotores");
        return;
    }
    if (!jsonGetString(body, "reflector", names[3], 8)) {
        jsonErr(out, outSize, "falta reflector");
        return;
    }
    if (!jsonGetString(body, "rings", rings, 8)) {
        copyStr(rings, sizeof(rings), "AAA");
    }
    jsonGetString(body, "plugboard", plugs, sizeof(plugs));
    const char *err = "";
    if (!gBombe.start(cipher, crib, alignment, names[0], names[1], names[2], names[3], rings, plugs, &err)) {
        jsonErr(out, outSize, err);
        gLeds.setYellow(false);
        return;
    }
    gLeds.setYellow(true);
    gLeds.setGreen(false);
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"state\":\"running\",\"total\":17576}");
}

void apiBombeStatus(char *out, int outSize) {
    bombe::BombeProgress p;
    gBombe.getProgress(p);
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"state\":\"");
    append(out, outSize, n, gBombe.stateName());
    append(out, outSize, n, "\",\"evaluated\":");
    appendInt(out, outSize, n, p.evaluated);
    append(out, outSize, n, ",\"discarded\":");
    appendInt(out, outSize, n, p.discarded);
    append(out, outSize, n, ",\"candidates\":");
    appendInt(out, outSize, n, p.candidates);
    append(out, outSize, n, ",\"verified\":");
    appendInt(out, outSize, n, p.verified);
    append(out, outSize, n, ",\"stored\":");
    appendInt(out, outSize, n, p.stored);
    append(out, outSize, n, ",\"total\":");
    appendInt(out, outSize, n, p.total);
    append(out, outSize, n, ",\"current\":\"");
    appendChar(out, outSize, n, enigma::indexLetter(p.currentL < 26 ? p.currentL : 25));
    appendChar(out, outSize, n, enigma::indexLetter(p.currentM));
    appendChar(out, outSize, n, enigma::indexLetter(p.currentR));
    append(out, outSize, n, "\",\"lastDiscard\":\"");
    appendChar(out, outSize, n, enigma::indexLetter(p.lastDiscardL));
    appendChar(out, outSize, n, enigma::indexLetter(p.lastDiscardM));
    appendChar(out, outSize, n, enigma::indexLetter(p.lastDiscardR));
    append(out, outSize, n, "\",\"lastReason\":\"");
    char esc[160];
    jsonEscape(bombe::discardReasonText(p.lastReason), esc, sizeof(esc));
    append(out, outSize, n, esc);
    append(out, outSize, n, "\",\"leds\":{\"yellow\":");
    append(out, outSize, n, gLeds.yellow() ? "true" : "false");
    append(out, outSize, n, ",\"red\":");
    append(out, outSize, n, gLeds.red() ? "true" : "false");
    append(out, outSize, n, ",\"green\":");
    append(out, outSize, n, gLeds.green() ? "true" : "false");
    append(out, outSize, n, "},\"error\":\"");
    jsonEscape(p.error ? p.error : "", esc, sizeof(esc));
    append(out, outSize, n, esc);
    append(out, outSize, n, "\"}");
}

void apiBombeCandidates(char *out, int outSize) {
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"items\":[");
    for (int i = 0; i < gBombe.candidateCount(); ++i) {
        if (i > 0) {
            append(out, outSize, n, ",");
        }
        const bombe::Candidate &c = gBombe.candidate(i);
        append(out, outSize, n, "{\"pos\":\"");
        append(out, outSize, n, c.posLetters);
        append(out, outSize, n, "\",\"verified\":");
        append(out, outSize, n, c.verified ? "true" : "false");
        append(out, outSize, n, ",\"recovered\":\"");
        char esc[MAX_CRIB_LEN * 2];
        jsonEscape(c.recovered, esc, sizeof(esc));
        append(out, outSize, n, esc);
        append(out, outSize, n, "\",\"stecker\":\"");
        char st[80];
        int sn = 0;
        st[0] = '\0';
        bool used[26] = {false};
        for (int a = 0; a < 26; ++a) {
            if (used[a] || c.stecker[a] < 0 || c.stecker[a] == a) {
                continue;
            }
            int b = c.stecker[a];
            if (b < a) {
                continue;
            }
            if (sn > 0 && sn + 1 < 80) {
                st[sn++] = ' ';
            }
            if (sn + 2 < 80) {
                st[sn++] = enigma::indexLetter(a);
                st[sn++] = enigma::indexLetter(b);
                st[sn] = '\0';
            }
            used[a] = true;
            used[b] = true;
        }
        jsonEscape(st, esc, sizeof(esc));
        append(out, outSize, n, esc);
        append(out, outSize, n, "\"}");
    }
    append(out, outSize, n, "]}");
}

void apiBombeCancel(char *out, int outSize) {
    gBombe.cancel();
    gLeds.setYellow(false);
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"state\":\"");
    append(out, outSize, n, gBombe.stateName());
    append(out, outSize, n, "\"}");
}

void apiBombeReset(char *out, int outSize) {
    gBombe.reset();
    gLeds.allOff();
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"state\":\"idle\"}");
}

void apiInfo(char *out, int outSize) {
    int n = 0;
    jsonOkPrefix(out, outSize, n);
    append(out, outSize, n, ",\"device\":\"Enigma+Bombe ESP32\",\"ssid\":\"");
    append(out, outSize, n, WIFI_AP_SSID);
    append(out, outSize, n, "\",\"rotors\":[");
    for (int i = 0; i < enigma::ROTOR_CATALOG_COUNT; ++i) {
        if (i) {
            append(out, outSize, n, ",");
        }
        append(out, outSize, n, "\"");
        append(out, outSize, n, enigma::ROTOR_CATALOG[i]->name);
        append(out, outSize, n, "\"");
    }
    append(out, outSize, n, "],\"reflectors\":[\"A\",\"B\",\"C\"]}");
}

}  // namespace web
