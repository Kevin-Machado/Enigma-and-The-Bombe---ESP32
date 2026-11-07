#include "json_util.h"

namespace web {

static bool keyMatch(const char *p, const char *key) {
    while (*key) {
        if (*p != *key) {
            return false;
        }
        ++p;
        ++key;
    }
    return *p == '"';
}

static const char *findKey(const char *body, const char *key) {
    if (body == nullptr || key == nullptr) {
        return nullptr;
    }
    for (int i = 0; body[i] != '\0'; ++i) {
        if (body[i] == '"' && keyMatch(body + i + 1, key)) {
            const char *p = body + i + 1;
            while (*p && *p != '"') {
                ++p;
            }
            if (*p == '"') {
                ++p;
            }
            while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
                ++p;
            }
            if (*p == ':') {
                ++p;
                while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
                    ++p;
                }
                return p;
            }
        }
    }
    return nullptr;
}

bool jsonGetString(const char *body, const char *key, char *out, int outSize) {
    if (outSize <= 0) {
        return false;
    }
    out[0] = '\0';
    const char *p = findKey(body, key);
    if (p == nullptr) {
        return false;
    }
    if (*p != '"') {
        int n = 0;
        while (*p && *p != ',' && *p != '}' && *p != ']' && n + 1 < outSize) {
            if (*p != ' ' && *p != '\n' && *p != '\r' && *p != '\t') {
                out[n++] = *p;
            }
            ++p;
        }
        out[n] = '\0';
        return n > 0;
    }
    ++p;
    int n = 0;
    while (*p && *p != '"' && n + 1 < outSize) {
        if (*p == '\\' && p[1]) {
            ++p;
        }
        out[n++] = *p++;
    }
    out[n] = '\0';
    return true;
}

bool jsonGetInt(const char *body, const char *key, int *out) {
    const char *p = findKey(body, key);
    if (p == nullptr || out == nullptr) {
        return false;
    }
    int sign = 1;
    if (*p == '-') {
        sign = -1;
        ++p;
    }
    if (*p < '0' || *p > '9') {
        return false;
    }
    int v = 0;
    while (*p >= '0' && *p <= '9') {
        v = v * 10 + (*p - '0');
        ++p;
    }
    *out = v * sign;
    return true;
}

bool jsonGetBool(const char *body, const char *key, bool *out) {
    const char *p = findKey(body, key);
    if (p == nullptr || out == nullptr) {
        return false;
    }
    if (p[0] == 't' && p[1] == 'r' && p[2] == 'u' && p[3] == 'e') {
        *out = true;
        return true;
    }
    if (p[0] == 'f' && p[1] == 'a' && p[2] == 'l' && p[3] == 's' && p[4] == 'e') {
        *out = false;
        return true;
    }
    return false;
}

bool jsonGetStringArray3(const char *body, const char *key, char out[3][8]) {
    const char *p = findKey(body, key);
    if (p == nullptr || *p != '[') {
        return false;
    }
    ++p;
    for (int k = 0; k < 3; ++k) {
        while (*p == ' ' || *p == ',') {
            ++p;
        }
        if (*p != '"') {
            return false;
        }
        ++p;
        int n = 0;
        while (*p && *p != '"' && n + 1 < 8) {
            out[k][n++] = *p++;
        }
        out[k][n] = '\0';
        if (*p == '"') {
            ++p;
        }
    }
    return true;
}

void jsonEscape(const char *in, char *out, int outSize) {
    if (outSize <= 0) {
        return;
    }
    int n = 0;
    if (in) {
        for (int i = 0; in[i] != '\0' && n + 2 < outSize; ++i) {
            char c = in[i];
            if (c == '"' || c == '\\') {
                out[n++] = '\\';
            }
            if (c == '\n') {
                if (n + 2 >= outSize) {
                    break;
                }
                out[n++] = '\\';
                out[n++] = 'n';
                continue;
            }
            out[n++] = c;
        }
    }
    out[n] = '\0';
}

void append(char *buf, int bufSize, int &n, const char *s) {
    if (!s) {
        return;
    }
    while (*s && n + 1 < bufSize) {
        buf[n++] = *s++;
    }
    buf[n] = '\0';
}

void appendInt(char *buf, int bufSize, int &n, int v) {
    char tmp[16];
    int i = 0;
    if (v < 0) {
        if (n + 1 < bufSize) {
            buf[n++] = '-';
        }
        v = -v;
    }
    if (v == 0) {
        tmp[i++] = '0';
    }
    while (v > 0 && i < 15) {
        tmp[i++] = static_cast<char>('0' + (v % 10));
        v /= 10;
    }
    while (i > 0 && n + 1 < bufSize) {
        buf[n++] = tmp[--i];
    }
    buf[n] = '\0';
}

void appendChar(char *buf, int bufSize, int &n, char c) {
    if (n + 1 < bufSize) {
        buf[n++] = c;
        buf[n] = '\0';
    }
}

}  // namespace web
