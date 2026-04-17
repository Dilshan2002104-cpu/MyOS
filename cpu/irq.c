

#include "irq.h"
#include "idt.h"
#include "gdt.h"
#include "../include/io.h"

extern void irq0(void);  extern void irq1(void);  extern void irq2(void);
extern void irq3(void);  extern void irq4(void);  extern void irq5(void);
extern void irq6(void);  extern void irq7(void);  extern void irq8(void);
extern void irq9(void);  extern void irq10(void); extern void irq11(void);
extern void irq12(void); extern void irq13(void); extern void irq14(void);
extern void irq15(void);

static isr_handler_t irq_handlers[16];

static void pic_remap(void)
{
    
    outb(PIC_MASTER_CMD,  0x11);
    io_wait();
    outb(PIC_SLAVE_CMD,   0x11);
    io_wait();

    outb(PIC_MASTER_DATA, 0x20);   
    io_wait();
    outb(PIC_SLAVE_DATA,  0x28);   
    io_wait();

    outb(PIC_MASTER_DATA, 0x04);   
    io_wait();
    outb(PIC_SLAVE_DATA,  0x02);   
    io_wait();

    outb(PIC_MASTER_DATA, 0x01);
    io_wait();
    outb(PIC_SLAVE_DATA,  0x01);
    io_wait();

    outb(PIC_MASTER_DATA, 0x00);
    outb(PIC_SLAVE_DATA,  0x00);
}

void irq_init(void)
{
    pic_remap();

    static void (*stubs[16])(void) = {
        irq0,  irq1,  irq2,  irq3,
        irq4,  irq5,  irq6,  irq7,
        irq8,  irq9,  irq10, irq11,
        irq12, irq13, irq14, irq15,
    };

    for (int i = 0; i < 16; i++)
        idt_set_gate((u8)(32 + i), (u32)stubs[i],
                     GDT_SEL_KCODE, IDT_GATE_INTERRUPT);
}

void irq_register_handler(u8 irq, isr_handler_t handler)
{
    if (irq < 16)
        irq_handlers[irq] = handler;
}

void irq_handler(registers_t *regs)
{
    u8 irq = (u8)(regs->int_no - 32);

    if (irq < 16 && irq_handlers[irq])
        irq_handlers[irq](regs);

    if (regs->int_no >= 40)
        outb(PIC_SLAVE_CMD, PIC_EOI);

    outb(PIC_MASTER_CMD, PIC_EOI);
}
