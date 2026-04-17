#pragma once
/* cpu/irq.h — Hardware IRQ handlers via 8259 PIC (Phase 2) */

#include "isr.h"   /* for isr_handler_t and registers_t */

/* IRQ numbers (0-15) — map to IDT vectors 32-47 after PIC remapping */
#define IRQ_TIMER     0
#define IRQ_KEYBOARD  1
#define IRQ_CASCADE   2   /* slave PIC — do not use directly */
#define IRQ_COM2      3
#define IRQ_COM1      4
#define IRQ_LPT2      5
#define IRQ_FLOPPY    6
#define IRQ_LPT1      7
#define IRQ_CMOS      8
#define IRQ_FREE1     9
#define IRQ_FREE2     10
#define IRQ_FREE3     11
#define IRQ_PS2MOUSE  12
#define IRQ_COPROC    13
#define IRQ_ATA1      14
#define IRQ_ATA2      15

/* PIC I/O ports */
#define PIC_MASTER_CMD   0x20
#define PIC_MASTER_DATA  0x21
#define PIC_SLAVE_CMD    0xA0
#define PIC_SLAVE_DATA   0xA1
#define PIC_EOI          0x20   /* End-of-Interrupt command */

void irq_init(void);

/* Register a C handler for a hardware IRQ line (0-15) */
void irq_register_handler(u8 irq, isr_handler_t handler);

/* The common C dispatcher — called from assembly stubs */
void irq_handler(registers_t *regs);
