#pragma once
/* mm/paging.h — Virtual Memory / Paging (Phase 3) */

#include "../include/types.h"

void paging_init(void);
void map_page(u32 phys, u32 virt, u32 flags);
void unmap_page(u32 virt);
