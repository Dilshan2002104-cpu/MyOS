#pragma once
/* mm/paging.h — Virtual Memory / Paging (Phase 3) */

#include "../include/types.h"

/* ── Page Directory/Table Entry Flags ────────────────────────────────────── */
#define PAGE_PRESENT    0x01
#define PAGE_WRITABLE   0x02
#define PAGE_USER       0x04
#define PAGE_ACCESSED   0x20
#define PAGE_DIRTY      0x40

/* Initialise paging and identity map the first 4MB */
void paging_init(void);

/* Map a virtual address to a physical address */
void paging_map(u32 virt, u32 phys, u32 flags);

/* Enable paging (loads CR3 and sets PG bit in CR0) */
void paging_enable(u32 page_directory_addr);
