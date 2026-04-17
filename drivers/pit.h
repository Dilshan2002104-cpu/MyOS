#pragma once
/* drivers/pit.h — Programmable Interval Timer driver (Phase 4) */

#include "../include/types.h"

void pit_init(u32 frequency_hz);
void pit_sleep_ms(u32 ms);
u32  pit_get_ticks(void);
