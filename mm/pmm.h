#pragma once
/* mm/pmm.h — Physical Memory Manager (Phase 3) */

#include "../include/types.h"

#define PAGE_SIZE       4096
#define PAGE_ALIGN(x)   ((x) & ~(PAGE_SIZE - 1))
#define PAGE_UP(x)      (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

/* Initialise the PMM using Multiboot memory map */
void pmm_init(u32 mmap_addr, u32 mmap_len, u32 kernel_end);

/* Allocate a single 4KB page frame (returns physical address) */
u32 pmm_alloc_frame(void);

/* Free a physical page frame */
void pmm_free_frame(u32 frame_addr);

/* Return number of free frames */
u32 pmm_get_free_frames(void);

/* Return total number of frames */
u32 pmm_get_total_frames(void);

/* Mark a range of memory as used (internal/init use) */
void pmm_mark_used(u32 start, u32 len);

/* Mark a range of memory as free (internal/init use) */
void pmm_mark_free(u32 start, u32 len);
