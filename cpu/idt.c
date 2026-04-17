/* cpu/idt.c — Interrupt Descriptor Table setup
 *
 * Fills all 256 IDT gates, then delegates:
 *   Vectors  0-31 → isr_init()  (CPU exceptions)
 *   Vectors 32-47 → irq_init()  (hardware IRQs via remapped PIC)
 *
 * Phase 2.
 */

#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "../include/types.h"

/* ── IDT storage ─────────────────────────────────────────────────────────── */
static idt_entry_t idt[256];
static idt_ptr_t   idt_ptr;

/* Declared in idt_flush.s */
extern void idt_flush(u32 idt_ptr_addr);

/* ── Public: set a single gate ───────────────────────────────────────────── */
void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags)
{
    idt[num].base_lo = (u16)(base & 0xFFFF);
    idt[num].base_hi = (u16)((base >> 16) & 0xFFFF);
    idt[num].sel     = sel;
    idt[num].always0 = 0;
    idt[num].flags   = flags;
}

/* ── Public: initialise and load the IDT ─────────────────────────────────── */
void idt_init(void)
{
    idt_ptr.limit = (u16)(sizeof(idt) - 1);
    idt_ptr.base  = (u32)&idt;

    /* Zero out all entries first */
    for (int i = 0; i < 256; i++)
        idt_set_gate((u8)i, 0, 0, 0);

    /* Install CPU exception handlers (vectors 0-31) */
    isr_init();

    /* Remap PIC and install hardware IRQ handlers (vectors 32-47) */
    irq_init();

    /* Load the IDTR */
    idt_flush((u32)&idt_ptr);
}
