#include "server.h"
#include "pages.h"
#include "api.h"
#include "../config/hardware_config.h"

#if defined(ARDUINO) && !defined(HOST_BUILD)
#include <WiFi.h>
#include <WebServer.h>
#endif

namespace web {

#if defined(ARDUINO) && !defined(HOST_BUILD)

static WebServer server(HTTP_SERVER_PORT);
static char bodyBuf[HTTP_MAX_BODY];
static char jsonBuf[4096];

static void readBody() {
    bodyBuf[0] = '\0';
    int n = 0;
    if (server.hasArg("plain")) {
        const String &p = server.arg("plain");
        int lim = p.length();
        if (lim >= HTTP_MAX_BODY) {
            lim = HTTP_MAX_BODY - 1;
        }
        for (int i = 0; i < lim; ++i) {
            bodyBuf[n++] = p[i];
        }
        bodyBuf[n] = '\0';
        return;
    }
}

static void sendJson() {
    server.sendHeader("Cache-Control", "no-store");
    server.send(200, "application/json", jsonBuf);
}

static void handleIndex() { server.send(200, "text/html", web::pageIndex()); }
static void handleEnigmaPage() { server.send(200, "text/html", web::pageEnigma()); }
static void handleBombePage() { server.send(200, "text/html", web::pageBombe()); }
static void handleCss() { server.send(200, "text/css", web::pageCss()); }
static void handleJs() { server.send(200, "application/javascript", web::pageJs()); }
static void handleInfo() { web::apiInfo(jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleEnigmaState() { web::apiEnigmaState(jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleEnigma() { readBody(); web::apiEnigma(bodyBuf, jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleEnigmaReset() { readBody(); web::apiEnigmaReset(bodyBuf, jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleBombeStart() { readBody(); web::apiBombeStart(bodyBuf, jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleBombeStatus() { web::apiBombeStatus(jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleBombeCands() { web::apiBombeCandidates(jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleBombeCancel() { web::apiBombeCancel(jsonBuf, sizeof(jsonBuf)); sendJson(); }
static void handleBombeReset() { web::apiBombeReset(jsonBuf, sizeof(jsonBuf)); sendJson(); }

void serverBegin() {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL);
    server.on("/", HTTP_GET, handleIndex);
    server.on("/enigma", HTTP_GET, handleEnigmaPage);
    server.on("/bombe", HTTP_GET, handleBombePage);
    server.on("/style.css", HTTP_GET, handleCss);
    server.on("/app.js", HTTP_GET, handleJs);
    server.on("/api/info", HTTP_GET, handleInfo);
    server.on("/api/enigma/state", HTTP_GET, handleEnigmaState);
    server.on("/api/enigma", HTTP_POST, handleEnigma);
    server.on("/api/enigma/reset", HTTP_POST, handleEnigmaReset);
    server.on("/api/bombe/start", HTTP_POST, handleBombeStart);
    server.on("/api/bombe/status", HTTP_GET, handleBombeStatus);
    server.on("/api/bombe/candidates", HTTP_GET, handleBombeCands);
    server.on("/api/bombe/cancel", HTTP_POST, handleBombeCancel);
    server.on("/api/bombe/reset", HTTP_POST, handleBombeReset);
    server.begin();
}

void serverLoop() {
    server.handleClient();
}

#else

void serverBegin() {}
void serverLoop() {}

#endif

}  // namespace web
