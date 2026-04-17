

#include "pit.h"
#include "../cpu/irq.h"
#include "../include/io.h"

static u32 pit_ticks = 0;
static u32 pit_freq = 0;

static void pit_callback(registers_t *regs) {
    (void)regs;
    pit_ticks++;
}

void pit_init(u32 frequency) {
    pit_freq = frequency;
    irq_register_handler(IRQ_TIMER, pit_callback);

    u32 divisor = PIT_BASE_FREQUENCY / frequency;

    outb(0x43, 0x36);

    outb(0x40, (u8)(divisor & 0xFF));
    outb(0x40, (u8)((divisor >> 8) & 0xFF));
}

u32 pit_get_ticks(void) {
    return pit_ticks;
}

void sleep(u32 ms) {
    u32 start_ticks = pit_ticks;
    
    u32 ticks_to_wait = (ms * pit_freq) / 1000;
    
    while (pit_ticks < start_ticks + ticks_to_wait) {
        __asm__ volatile("hlt"); 
    }
}
