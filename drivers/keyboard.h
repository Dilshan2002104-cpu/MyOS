#pragma once
/* drivers/keyboard.h — PS/2 Keyboard driver (Phase 4) */

#include "../include/types.h"

void keyboard_init(void);
char keyboard_getchar(void);   /* blocks until a key is available */
bool keyboard_has_char(void);
