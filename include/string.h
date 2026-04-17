#pragma once

#include "types.h"

void *memcpy(void *dest, const void *src, u32 n);
void *memset(void *s, int c, u32 n);
u32 strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
