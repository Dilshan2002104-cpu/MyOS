#pragma once
/* cpu/irq.h — Hardware IRQ handlers via 8259 PIC (Phase 2) */

#include "isr.h"

void irq_init(void);
void irq_register_handler(u8 irq_num, isr_handler_t handler);
