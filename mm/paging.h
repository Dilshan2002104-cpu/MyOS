#pragma once

#include "../include/types.h"

#define PAGE_PRESENT    0x01
#define PAGE_WRITABLE   0x02
#define PAGE_USER       0x04
#define PAGE_ACCESSED   0x20
#define PAGE_DIRTY      0x40

void paging_init(void);

void paging_map(u32 virt, u32 phys, u32 flags);

void paging_enable(u32 page_directory_addr);

u32 paging_get_kernel_pd(void);
