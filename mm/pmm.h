#pragma once
/* mm/pmm.h — Physical Memory Manager (Phase 3) */

#include "../include/types.h"

void   pmm_init(u32 mem_size_kb, u32 *bitmap_addr);
u32    pmm_alloc_frame(void);
void   pmm_free_frame(u32 frame_addr);
u32    pmm_free_frames(void);
