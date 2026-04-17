#pragma once

#include "../include/types.h"

void heap_init(u32 start_addr, u32 size);

void *kmalloc(u32 size);

void *kmalloc_aligned(u32 size);

void kfree(void *ptr);

void heap_dump(void);
