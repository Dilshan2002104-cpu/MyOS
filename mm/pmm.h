#pragma once

#include "../include/types.h"

#define PAGE_SIZE       4096
#define PAGE_ALIGN(x)   ((x) & ~(PAGE_SIZE - 1))
#define PAGE_UP(x)      (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

void pmm_init(u32 mmap_addr, u32 mmap_len, u32 kernel_end);

u32 pmm_alloc_frame(void);

void pmm_free_frame(u32 frame_addr);

u32 pmm_get_free_frames(void);

u32 pmm_get_total_frames(void);

void pmm_mark_used(u32 start, u32 len);

void pmm_mark_free(u32 start, u32 len);
