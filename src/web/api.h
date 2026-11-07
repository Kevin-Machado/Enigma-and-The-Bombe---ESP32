#ifndef WEB_API_H
#define WEB_API_H

namespace web {

void apiInit();
void apiTick();
void apiEnigma(const char *body, char *out, int outSize);
void apiEnigmaReset(const char *body, char *out, int outSize);
void apiEnigmaState(char *out, int outSize);
void apiBombeStart(const char *body, char *out, int outSize);
void apiBombeStatus(char *out, int outSize);
void apiBombeCandidates(char *out, int outSize);
void apiBombeCancel(char *out, int outSize);
void apiBombeReset(char *out, int outSize);
void apiInfo(char *out, int outSize);

}  // namespace web

#endif
