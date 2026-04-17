#pragma once

#include "../include/types.h"
#include "../cpu/isr.h"

#define PIT_BASE_FREQUENCY 1193180

void pit_init(u32 frequency);

void sleep(u32 ms);

u32 pit_get_ticks(void);
