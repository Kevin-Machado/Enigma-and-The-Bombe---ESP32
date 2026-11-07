#include "web/api.h"
#include "web/server.h"
#include "config/hardware_config.h"

#if defined(ARDUINO) && !defined(HOST_BUILD)
#include <Arduino.h>
#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.println("Enigma + The Bombe");
    Serial.print("AP SSID: ");
    Serial.println(WIFI_AP_SSID);
    web::apiInit();
    web::serverBegin();
    Serial.print("URL: http://");
    Serial.println(WiFi.softAPIP());
}

void loop() {
    web::serverLoop();
    web::apiTick();
}

#endif
