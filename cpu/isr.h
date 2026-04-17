#pragma once

#include "../include/types.h"

typedef struct {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_no, err_code;
    u32 eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;

typedef void (*isr_handler_t)(registers_t *);

void isr_init(void);

void isr_register_handler(u8 isr_num, isr_handler_t handler);

void isr_handler(registers_t *regs);
