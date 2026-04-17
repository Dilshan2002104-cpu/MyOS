

#include "../include/string.h"

void *memcpy(void *dest, const void *src, u32 n) {
    u8 *d = (u8 *)dest;
    const u8 *s = (const u8 *)src;
    for (u32 i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void *memset(void *s, int c, u32 n) {
    u8 *p = (u8 *)s;
    for (u32 i = 0; i < n; i++) p[i] = (u8)c;
    return s;
}

u32 strlen(const char *s) {
    u32 i = 0;
    while (s[i] != '\0') i++;
    return i;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}
