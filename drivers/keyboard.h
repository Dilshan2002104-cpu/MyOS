#pragma once

#include "../include/types.h"

void keyboard_init(void);

char getchar(void);

void gets(char *buffer, u32 size);

bool keyboard_has_char(void);
