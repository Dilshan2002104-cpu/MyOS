#pragma once
/* mm/heap.h — Kernel heap allocator (Phase 3) */

#include "../include/types.h"

void  heap_init(u32 start_addr, u32 size);
void *kmalloc(u32 size);
void *kmalloc_aligned(u32 size);   /* page-aligned */
void  kfree(void *ptr);
