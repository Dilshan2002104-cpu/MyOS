#pragma once
/* cpu/idt.h — Interrupt Descriptor Table (Phase 2) */

#include "../include/types.h"

/* A single IDT gate descriptor — 8 bytes                                 */
typedef struct {
    u16 base_lo;    /* handler offset bits 15:0                           */
    u16 sel;        /* segment selector (GDT_SEL_KCODE = 0x08)           */
    u8  always0;    /* must be 0                                          */
    u8  flags;      /* P | DPL[1:0] | 0 | Type[3:0]                     */
    u16 base_hi;    /* handler offset bits 31:16                          */
} __attribute__((packed)) idt_entry_t;

/* IDTR — the 6-byte value loaded by lidt                                 */
typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_ptr_t;

/* Gate type flags:
 *   0x8E = 1000_1110 — present, DPL=0, 32-bit interrupt gate
 *   0x8F = 1000_1111 — present, DPL=0, 32-bit trap gate (no CLI)
 *   0xEE = 1110_1110 — present, DPL=3, for user-space int 0x80 syscalls
 */
#define IDT_GATE_INTERRUPT  0x8E
#define IDT_GATE_TRAP       0x8F
#define IDT_GATE_SYSCALL    0xEE

void idt_init(void);
void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags);
