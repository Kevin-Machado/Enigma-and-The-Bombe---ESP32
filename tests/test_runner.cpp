#include <cstdio>
#include <cstring>
#include <cstdlib>
#include "../src/enigma/enigma_machine.h"
#include "../src/enigma/rotor.h"
#include "../src/enigma/plugboard.h"
#include "../src/enigma/reflector.h"
#include "../src/bombe/menu.h"
#include "../src/bombe/constraints.h"
#include "../src/bombe/bombe_engine.h"
#include "../src/bombe/verifier.h"
#include "../src/web/api.h"
#include "../src/web/json_util.h"
#include "../src/hardware/leds.h"

static int gPass = 0;
static int gFail = 0;

static void expect(bool cond, const char *name) {
    if (cond) {
        ++gPass;
        std::printf("PASS  %s\n", name);
    } else {
        ++gFail;
        std::printf("FAIL  %s\n", name);
    }
}

static void testWirings() {
    for (int i = 0; i < enigma::ROTOR_CATALOG_COUNT; ++i) {
        uint8_t fwd[26];
        uint8_t inv[26];
        bool ok = enigma::parseWiring(enigma::ROTOR_CATALOG[i]->wiring, fwd);
        expect(ok, enigma::ROTOR_CATALOG[i]->name);
        if (!ok) {
            continue;
        }
        enigma::invertPerm(fwd, inv);
        bool round = true;
        for (int a = 0; a < 26; ++a) {
            if (inv[fwd[a]] != a || fwd[inv[a]] != a) {
                round = false;
            }
        }
        char buf[40];
        std::snprintf(buf, sizeof(buf), "inverso %s", enigma::ROTOR_CATALOG[i]->name);
        expect(round, buf);
    }
    for (int i = 0; i < enigma::REFLECTOR_CATALOG_COUNT; ++i) {
        enigma::Reflector r;
        expect(r.configure(enigma::REFLECTOR_CATALOG[i]), enigma::REFLECTOR_CATALOG[i]->name);
        bool invol = true;
        bool nofix = true;
        for (int a = 0; a < 26; ++a) {
            int b = r.map(a);
            if (b == a) {
                nofix = false;
            }
            if (r.map(b) != a) {
                invol = false;
            }
        }
        char buf[48];
        std::snprintf(buf, sizeof(buf), "reflector involucion %s", enigma::REFLECTOR_CATALOG[i]->name);
        expect(invol && nofix, buf);
    }
}

static void testPlugboard() {
    enigma::Plugboard p;
    expect(p.parse("AB CD EF"), "plugboard pares");
    expect(p.map(0) == 1 && p.map(1) == 0, "plugboard AB");
    expect(p.map(2) == 3 && p.map(25) == 25, "plugboard resto identidad");
    expect(!p.parse("AA"), "plugboard rechaza letra consigo misma");
    expect(!p.parse("AB AC"), "plugboard rechaza letra repetida");
    expect(p.parse(""), "plugboard vacio");
}

static void testDoubleStepping() {
    enigma::EnigmaMachine m;
    expect(m.configure("I", "II", "III", "B", 0, 0, 0, 0, 3, 20, ""), "config doble paso");
    int l, mid, r;
    m.encodeLetter(0);
    m.getPositions(l, mid, r);
    expect(l == 0 && mid == 3 && r == 21, "paso 1: A D V");
    m.encodeLetter(0);
    m.getPositions(l, mid, r);
    expect(l == 0 && mid == 4 && r == 22, "paso 2: medio avanza por muesca derecha A E W");
    m.encodeLetter(0);
    m.getPositions(l, mid, r);
    expect(l == 1 && mid == 5 && r == 23, "paso 3: doble paso B F X");
}

static void testNoSelf() {
    enigma::EnigmaMachine m;
    m.configure("I", "II", "III", "B", 0, 0, 0, 0, 0, 0, "");
    bool ok = true;
    for (int i = 0; i < 26; ++i) {
        enigma::EnigmaMachine t;
        t.configure("I", "II", "III", "B", 0, 0, 0, 0, 0, 0, "");
        if (t.encodeLetter(i) == i) {
            ok = false;
        }
    }
    expect(ok, "ninguna letra se cifra como si misma en AAA");
}

static void testReciprocity() {
    const char *plain = "WETTERVORHERSAGEBERLINXX";
    enigma::EnigmaMachine enc;
    enc.configure("I", "II", "III", "B", 0, 0, 0, 5, 11, 2, "AB CD EF");
    char cipher[64];
    enc.processLettersOnly(plain, cipher, sizeof(cipher));
    enigma::EnigmaMachine dec;
    dec.configure("I", "II", "III", "B", 0, 0, 0, 5, 11, 2, "AB CD EF");
    char back[64];
    dec.processLettersOnly(cipher, back, sizeof(back));
    expect(std::strcmp(plain, back) == 0, "cifrado+descifrado reciproco");
}

static void testSpacesAndJunk() {
    enigma::EnigmaMachine m;
    m.configure("I", "II", "III", "B", 0, 0, 0, 0, 0, 0, "");
    char out[32];
    int ign = 0;
    m.processText("AB 12 C!", out, sizeof(out), &ign);
    expect(std::strlen(out) == 3, "espacios ignorados, solo letras");
    expect(ign == 3, "numeros y simbolos contados como ignorados");
}

static void testKnownVectorInternal() {
    enigma::EnigmaMachine m;
    m.configure("I", "II", "III", "B", 0, 0, 0, 0, 0, 0, "");
    char out[32];
    m.processLettersOnly("AAAAA", out, sizeof(out));
    enigma::EnigmaMachine d;
    d.configure("I", "II", "III", "B", 0, 0, 0, 0, 0, 0, "");
    char back[32];
    d.processLettersOnly(out, back, sizeof(back));
    expect(std::strcmp(back, "AAAAA") == 0, "vector AAA/AAAAA reciproco");
    expect(std::strcmp(out, "AAAAA") != 0, "AAAAA no es punto fijo");
    expect(std::strcmp(out, "BDZGO") == 0, "vector historico I-II-III B AAA/AAA AAAAA -> BDZGO");
    std::printf("INFO  I II III B rings AAA pos AAA  AAAAA -> %s\n", out);
}

static void testMenu() {
    bombe::CryptoMenu menu;
    const char *err = "";
    expect(!menu.build("ABC", "ABC", 0, &err), "menu rechaza auto-cifrado");
    expect(menu.build("BDZGOW", "XXXXXX", 0, &err), "menu valido");
    expect(menu.linkCount() == 6, "menu 6 enlaces");
    expect(!menu.build("ABCDE", "HELLO", 2, &err), "alineacion fuera de rango");
}

static void testConstraintsContradiction() {
    bombe::CryptoMenu menu;
    const char *err = "";
    menu.build("ABCDEF", "GHIJKL", 0, &err);
    uint8_t fwd[MAX_CRIB_LEN][26];
    for (int t = 0; t < 6; ++t) {
        enigma::fillIdentity(fwd[t]);
    }
    int s[26];
    for (int i = 0; i < 26; ++i) {
        s[i] = -1;
    }
    bombe::ConstraintEngine::assignStecker(s, 0, 1);
    bombe::DiscardReason r = bombe::ConstraintEngine::propagate(&menu, fwd, s);
    expect(r != bombe::DISCARD_NONE, "permutacion identidad provoca contradiccion de stecker");
}

static void testBombeFindsKey() {
    const char *plain = "WETTER";
    enigma::EnigmaMachine enc;
    enc.configure("I", "II", "III", "B", 0, 0, 0, 0, 0, 0, "");
    char cipher[32];
    enc.processLettersOnly(plain, cipher, sizeof(cipher));
    std::printf("INFO  crib WETTER cifrado AAA -> %s\n", cipher);

    bombe::BombeEngine b;
    const char *err = "";
    expect(b.start(cipher, plain, 0, "I", "II", "III", "B", "AAA", "", &err), "bombe start");
    while (b.isRunning()) {
        b.stepSlice(64);
    }
    bombe::BombeProgress p;
    b.getProgress(p);
    expect(p.evaluated == 17576, "17576 posiciones evaluadas");
    expect(p.verified >= 1, "al menos un candidato verificado");
    bool foundAAA = false;
    for (int i = 0; i < b.candidateCount(); ++i) {
        const bombe::Candidate &c = b.candidate(i);
        if (c.posL == 0 && c.posM == 0 && c.posR == 0 && c.verified) {
            foundAAA = true;
            expect(std::strcmp(c.recovered, plain) == 0, "crib recuperado en AAA");
        }
    }
    expect(foundAAA, "posicion AAA verificada");
    expect(b.greenOn(), "LED verde logico tras verificacion");
    std::printf("INFO  evaluadas=%d descartadas=%d candidatas=%d verificadas=%d\n",
                p.evaluated, p.discarded, p.candidates, p.verified);
}

static void testBombeNoCandidate() {
    bombe::BombeEngine b;
    const char *err = "";
    bool started = b.start("BDZGOWCXLT", "WETTER", 0, "I", "II", "III", "B", "AAA", "", &err);
    if (!started) {
        expect(true, "crib incompatible rechazado en menu (auto-cifrado o rango)");
        return;
    }
    int steps = 0;
    while (b.isRunning() && steps < 400) {
        b.stepSlice(50);
        ++steps;
    }
    b.cancel();
    expect(b.isDone(), "busqueda cancelable");
}

static void testBombeCancelReset() {
    bombe::BombeEngine b;
    const char *err = "";
    b.start("QMJIDO", "ENIGMA", 0, "I", "II", "III", "B", "AAA", "", &err);
    b.stepSlice(20);
    b.cancel();
    bombe::BombeProgress p;
    b.getProgress(p);
    expect(p.state == bombe::BOMBE_CANCELLED, "estado cancelado");
    b.reset();
    b.getProgress(p);
    expect(p.state == bombe::BOMBE_IDLE && p.evaluated == 0, "reset a idle");
}

static void testVerifierIndependent() {
    enigma::MachineConfig cfg;
    cfg.rotorLeft = "I";
    cfg.rotorMiddle = "II";
    cfg.rotorRight = "III";
    cfg.reflector = "B";
    cfg.ringLeft = cfg.ringMiddle = cfg.ringRight = 'A';
    cfg.posLeft = cfg.posMiddle = cfg.posRight = 'A';
    cfg.plugboard = "";
    enigma::EnigmaMachine user;
    user.configure(cfg);
    user.encodeLetter(0);
    int l1, m1, r1;
    user.getPositions(l1, m1, r1);
    bombe::CryptoMenu menu;
    const char *err = "";
    enigma::EnigmaMachine src;
    src.configure(cfg);
    char cipher[16];
    src.processLettersOnly("HELLOW", cipher, sizeof(cipher));
    menu.build(cipher, "HELLOW", 0, &err);
    char rec[16];
    bool v = bombe::Verifier::verifyCrib(cfg, 0, 0, 0, menu, rec, sizeof(rec));
    expect(v, "verificador confirma crib");
    int l2, m2, r2;
    user.getPositions(l2, m2, r2);
    expect(l1 == l2 && m1 == m2 && r1 == r2, "verificador no altera Enigma del usuario");
}

static void testApiJson() {
    web::apiInit();
    char out[2048];
    web::apiInfo(out, sizeof(out));
    expect(std::strstr(out, "\"ok\":true") != nullptr, "api info ok");
    const char *body =
        "{\"rotorLeft\":\"I\",\"rotorMiddle\":\"II\",\"rotorRight\":\"III\","
        "\"reflector\":\"B\",\"rings\":\"AAA\",\"positions\":\"AAA\","
        "\"plugboard\":\"\",\"text\":\"HELLO\"}";
    web::apiEnigma(body, out, sizeof(out));
    expect(std::strstr(out, "\"ok\":true") != nullptr, "api enigma ok");
    char result[64];
    web::jsonGetString(out, "result", result, sizeof(result));
    expect(std::strlen(result) == 5, "api enigma 5 letras");
    web::apiBombeReset(out, sizeof(out));
    const char *bstart =
        "{\"rotorLeft\":\"I\",\"rotorMiddle\":\"II\",\"rotorRight\":\"III\","
        "\"reflector\":\"B\",\"rings\":\"AAA\",\"plugboard\":\"\","
        "\"ciphertext\":\"XXXXXX\",\"crib\":\"HELLOX\",\"alignment\":0}";
    web::apiBombeStart(bstart, out, sizeof(out));
    bool okStart = std::strstr(out, "\"ok\":true") != nullptr;
    if (!okStart) {
        expect(std::strstr(out, "ok\":false") != nullptr, "api bombe valida crib imposible");
    } else {
        web::apiTick();
        web::apiBombeStatus(out, sizeof(out));
        expect(std::strstr(out, "evaluated") != nullptr, "api status");
        web::apiBombeCancel(out, sizeof(out));
    }
    web::apiBombeReset(out, sizeof(out));
}

static void testLeds() {
    hardware::BombeLeds leds;
    leds.begin();
    leds.setYellow(true);
    expect(leds.yellow(), "LED amarillo encendido");
    hardware::BombeLeds::setHostMillis(0);
    leds.pulseRed();
    expect(leds.red(), "LED rojo pulso");
    hardware::BombeLeds::setHostMillis(RED_BLINK_MS + 1);
    leds.tick();
    expect(!leds.red(), "LED rojo se apaga");
    leds.setGreen(true);
    expect(leds.green(), "LED verde encendido");
    leds.allOff();
    expect(!leds.yellow() && !leds.green() && !leds.red(), "todos apagados");
}

static void testRingSettingsAffect() {
    enigma::EnigmaMachine a, b;
    a.configure("I", "II", "III", "B", 0, 0, 0, 0, 0, 0, "");
    b.configure("I", "II", "III", "B", 1, 0, 0, 0, 0, 0, "");
    char oa[8], ob[8];
    a.processLettersOnly("AAA", oa, sizeof(oa));
    b.processLettersOnly("AAA", ob, sizeof(ob));
    expect(std::strcmp(oa, ob) != 0, "ring setting cambia el cifrado");
}

int main() {
    std::printf("=== Pruebas Enigma + The Bombe (host) ===\n");
    testWirings();
    testPlugboard();
    testDoubleStepping();
    testNoSelf();
    testReciprocity();
    testSpacesAndJunk();
    testKnownVectorInternal();
    testRingSettingsAffect();
    testMenu();
    testConstraintsContradiction();
    testVerifierIndependent();
    testBombeCancelReset();
    testBombeNoCandidate();
    testLeds();
    testApiJson();
    testBombeFindsKey();
    std::printf("\n%d pass, %d fail\n", gPass, gFail);
    return gFail ? 1 : 0;
}
