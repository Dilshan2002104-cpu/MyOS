#pragma once
/* cpu/isr.h — CPU Exception (ISR) handlers (Phase 2) */

#include "../include/types.h"

/* Saved register state pushed by ISR stubs */
typedef struct {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; /* pusha */
    u32 int_no, err_code;
    u32 eip, cs, eflags, useresp, ss;            /* pushed by CPU */
} registers_t;

typedef void (*isr_handler_t)(registers_t *);

void isr_init(void);
void isr_register_handler(u8 isr_num, isr_handler_t handler);
