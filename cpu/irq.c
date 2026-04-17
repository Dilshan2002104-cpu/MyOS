/* cpu/irq.c — Hardware IRQ handler dispatcher + 8259 PIC management
 *
 * The 8259 PIC's default IRQ-to-vector mapping (0→INT8, 1→INT9, …)
 * clashes with CPU exception vectors 8-15. We remap them to 32-47.
 *
 * PIC remapping uses ICW1-ICW4 initialisation sequence:
 *   ICW1 → start init, edge-triggered, cascade
 *   ICW2 → new base vector offset
 *   ICW3 → cascade wiring between master and slave
 *   ICW4 → 8086 mode
 *
 * Phase 2.
 */

#include "irq.h"
#include "idt.h"
#include "gdt.h"
#include "../include/io.h"

/* Forward declarations of all 16 IRQ stubs (defined in irq.s) */
extern void irq0(void);  extern void irq1(void);  extern void irq2(void);
extern void irq3(void);  extern void irq4(void);  extern void irq5(void);
extern void irq6(void);  extern void irq7(void);  extern void irq8(void);
extern void irq9(void);  extern void irq10(void); extern void irq11(void);
extern void irq12(void); extern void irq13(void); extern void irq14(void);
extern void irq15(void);

/* ── Custom IRQ handler table ─────────────────────────────────────────────── */
static isr_handler_t irq_handlers[16];

/* ── Internal: remap the 8259 PIC ────────────────────────────────────────── */
static void pic_remap(void)
{
    /* ICW1: begin initialisation, cascade, edge-triggered */
    outb(PIC_MASTER_CMD,  0x11);
    io_wait();
    outb(PIC_SLAVE_CMD,   0x11);
    io_wait();

    /* ICW2: vector offsets — master IRQ0 → vector 32, slave IRQ8 → vector 40 */
    outb(PIC_MASTER_DATA, 0x20);   /* master: base = 32 */
    io_wait();
    outb(PIC_SLAVE_DATA,  0x28);   /* slave:  base = 40 */
    io_wait();

    /* ICW3: tell master that slave is on IRQ2; tell slave its cascade id */
    outb(PIC_MASTER_DATA, 0x04);   /* master: slave at IRQ2 (bit 2) */
    io_wait();
    outb(PIC_SLAVE_DATA,  0x02);   /* slave:  cascade identity = 2  */
    io_wait();

    /* ICW4: 8086/88 mode */
    outb(PIC_MASTER_DATA, 0x01);
    io_wait();
    outb(PIC_SLAVE_DATA,  0x01);
    io_wait();

    /* Unmask all IRQs on both PICs (0x00 = all enabled) */
    outb(PIC_MASTER_DATA, 0x00);
    outb(PIC_SLAVE_DATA,  0x00);
}

/* ── Public: initialise PIC and install IRQ gates ────────────────────────── */
void irq_init(void)
{
    pic_remap();

    static void (*stubs[16])(void) = {
        irq0,  irq1,  irq2,  irq3,
        irq4,  irq5,  irq6,  irq7,
        irq8,  irq9,  irq10, irq11,
        irq12, irq13, irq14, irq15,
    };

    /* Install IRQ gates at vectors 32-47 */
    for (int i = 0; i < 16; i++)
        idt_set_gate((u8)(32 + i), (u32)stubs[i],
                     GDT_SEL_KCODE, IDT_GATE_INTERRUPT);
}

/* ── Public: register a custom handler for an IRQ line ───────────────────── */
void irq_register_handler(u8 irq, isr_handler_t handler)
{
    if (irq < 16)
        irq_handlers[irq] = handler;
}

/* ── Public: common C-level IRQ dispatcher ───────────────────────────────── */
void irq_handler(registers_t *regs)
{
    u8 irq = (u8)(regs->int_no - 32);

    /* Dispatch to registered handler (if any) */
    if (irq < 16 && irq_handlers[irq])
        irq_handlers[irq](regs);

    /* Send End-of-Interrupt to acknowledgement PIC(s)
     * Slave PIC must be acknowledged first for IRQs 8-15. */
    if (regs->int_no >= 40)
        outb(PIC_SLAVE_CMD, PIC_EOI);

    outb(PIC_MASTER_CMD, PIC_EOI);
}
