#pragma once

#include "isr.h"   

#define IRQ_TIMER     0
#define IRQ_KEYBOARD  1
#define IRQ_CASCADE   2   
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

#define PIC_MASTER_CMD   0x20
#define PIC_MASTER_DATA  0x21
#define PIC_SLAVE_CMD    0xA0
#define PIC_SLAVE_DATA   0xA1
#define PIC_EOI          0x20   

void irq_init(void);

void irq_register_handler(u8 irq, isr_handler_t handler);

void irq_handler(registers_t *regs);
