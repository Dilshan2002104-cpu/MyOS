#pragma once
/* kernel/kprintf.h — Kernel formatted print (Phase 1) */

/* Supported format specifiers:
 *   %c  — single character
 *   %s  — null-terminated string
 *   %d  — signed 32-bit decimal
 *   %u  — unsigned 32-bit decimal
 *   %x  — unsigned 32-bit hex (lowercase)
 *   %X  — unsigned 32-bit hex (uppercase)
 *   %p  — pointer (0x-prefixed hex)
 *   %%  — literal percent sign
 *
 * Width and padding are NOT supported (keep it simple for Phase 1).
 */

void kprintf(const char *fmt, ...);
void kputs(const char *str);
void kputchar(char c);
