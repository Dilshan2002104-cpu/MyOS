#pragma once

#include "../include/types.h"

typedef struct {
    u16 base_lo;    
    u16 sel;        
    u8  always0;    
    u8  flags;      
    u16 base_hi;    
} __attribute__((packed)) idt_entry_t;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_ptr_t;

#define IDT_GATE_INTERRUPT  0x8E
#define IDT_GATE_TRAP       0x8F
#define IDT_GATE_SYSCALL    0xEE

void idt_init(void);
void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags);
