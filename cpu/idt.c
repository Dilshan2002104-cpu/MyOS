

#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "../include/types.h"

static idt_entry_t idt[256];
static idt_ptr_t   idt_ptr;

extern void idt_flush(u32 idt_ptr_addr);

void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags)
{
    idt[num].base_lo = (u16)(base & 0xFFFF);
    idt[num].base_hi = (u16)((base >> 16) & 0xFFFF);
    idt[num].sel     = sel;
    idt[num].always0 = 0;
    idt[num].flags   = flags;
}

void idt_init(void)
{
    idt_ptr.limit = (u16)(sizeof(idt) - 1);
    idt_ptr.base  = (u32)&idt;

    for (int i = 0; i < 256; i++)
        idt_set_gate((u8)i, 0, 0, 0);

    isr_init();

    irq_init();

    idt_flush((u32)&idt_ptr);
}
