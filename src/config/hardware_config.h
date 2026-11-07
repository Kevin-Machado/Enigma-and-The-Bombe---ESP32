#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H

// Pines GPIO de los tres LED de The Bombe.
// Completar segun el cableado real del ESP32 antes de la exposicion.
// Polaridad activa en ALTO (LED anodo al pin, catodo a GND via resistencia).

#ifndef BOMBE_LED_YELLOW_PIN
#define BOMBE_LED_YELLOW_PIN 25
#endif

#ifndef BOMBE_LED_RED_PIN
#define BOMBE_LED_RED_PIN 26
#endif

#ifndef BOMBE_LED_GREEN_PIN
#define BOMBE_LED_GREEN_PIN 27
#endif

// Resistencias limitadoras recomendadas: 220 ohm (rojo) o 330 ohm.
// Ver docs/CONEXIONES_LED.md

#ifndef LED_ACTIVE_HIGH
#define LED_ACTIVE_HIGH 1
#endif

#ifndef RED_BLINK_MS
#define RED_BLINK_MS 80
#endif

#ifndef WIFI_AP_SSID
#define WIFI_AP_SSID "EnigmaBombe"
#endif

#ifndef WIFI_AP_PASSWORD
#define WIFI_AP_PASSWORD "turing1939"
#endif

#ifndef WIFI_AP_CHANNEL
#define WIFI_AP_CHANNEL 6
#endif

#ifndef HTTP_SERVER_PORT
#define HTTP_SERVER_PORT 80
#endif

#ifndef BOMBE_STEPS_PER_SLICE
#define BOMBE_STEPS_PER_SLICE 16
#endif

#ifndef HTTP_MAX_BODY
#define HTTP_MAX_BODY 2048
#endif

#ifndef MAX_MESSAGE_LEN
#define MAX_MESSAGE_LEN 256
#endif

#ifndef MAX_CRIB_LEN
#define MAX_CRIB_LEN 32
#endif

#ifndef MAX_CANDIDATES
#define MAX_CANDIDATES 32
#endif

#ifndef MAX_MENU_LINKS
#define MAX_MENU_LINKS 32
#endif

#endif
