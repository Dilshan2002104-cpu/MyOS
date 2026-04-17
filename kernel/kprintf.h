#pragma once
/* kernel/kprintf.h — Kernel formatted print (Phase 1) */

void kprintf(const char *fmt, ...);
void kputs(const char *str);
void kputchar(char c);
