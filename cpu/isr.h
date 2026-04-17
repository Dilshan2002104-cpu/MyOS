#pragma once
/* cpu/isr.h — CPU Exception (ISR) handlers (Phase 2) */

#include "../include/types.h"

/* ── Register frame saved by ISR stubs ─────────────────────────────────────
 *
 * Stack layout built by our stubs (low address → high address):
 *
 *   [ESP+0 ]  ds         ← saved by stub (push eax after mov ax,ds)
 *   [ESP+4 ]  edi        ┐
 *   [ESP+8 ]  esi        │
 *   [ESP+12]  ebp        │ pusha — pushed in this stack order
 *   [ESP+16]  esp        │ (dummy; original ESP value captured by pusha)
 *   [ESP+20]  ebx        │
 *   [ESP+24]  edx        │
 *   [ESP+28]  ecx        │
 *   [ESP+32]  eax        ┘
 *   [ESP+36]  int_no     ← pushed by our stub
 *   [ESP+40]  err_code   ← pushed by CPU or dummy 0 by our stub
 *   [ESP+44]  eip        ┐
 *   [ESP+48]  cs         │ pushed by CPU automatically
 *   [ESP+52]  eflags     ┘
 *   [ESP+56]  useresp    ← only valid on ring-change (ring 3 → ring 0)
 *   [ESP+60]  ss         ← only valid on ring-change
 */
typedef struct {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 int_no, err_code;
    u32 eip, cs, eflags, useresp, ss;
} __attribute__((packed)) registers_t;

typedef void (*isr_handler_t)(registers_t *);

/* Install all 32 CPU exception gate entries into the IDT */
void isr_init(void);

/* Register a custom C handler for a specific exception vector (0-31) */
void isr_register_handler(u8 isr_num, isr_handler_t handler);

/* The common C dispatcher — called from assembly stubs */
void isr_handler(registers_t *regs);
