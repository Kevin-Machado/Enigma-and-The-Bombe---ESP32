#ifndef JSON_UTIL_H
#define JSON_UTIL_H

#include <stddef.h>

namespace web {

bool jsonGetString(const char *body, const char *key, char *out, int outSize);
bool jsonGetInt(const char *body, const char *key, int *out);
bool jsonGetBool(const char *body, const char *key, bool *out);
bool jsonGetStringArray3(const char *body, const char *key, char out[3][8]);
void jsonEscape(const char *in, char *out, int outSize);
void append(char *buf, int bufSize, int &n, const char *s);
void appendInt(char *buf, int bufSize, int &n, int v);
void appendChar(char *buf, int bufSize, int &n, char c);

}  // namespace web

#endif
