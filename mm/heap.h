#pragma once
/* mm/heap.h — Kernel Heap Allocator (Phase 3) */

#include "../include/types.h"

/* Initialise the kernel heap */
void heap_init(u32 start_addr, u32 size);

/* Allocate a block of memory */
void *kmalloc(u32 size);

/* Allocate a page-aligned block of memory */
void *kmalloc_aligned(u32 size);

/* Free an allocated block */
void kfree(void *ptr);

/* Debug: print current heap status */
void heap_dump(void);
