/* mm/paging.c — Virtual Memory Management
 *
 * Implements 2-level paging on x86.
 * Identity maps the first 4MB to keep the kernel running.
 *
 * Phase 3.
 */

#include "paging.h"
#include "pmm.h"
#include "../kernel/kprintf.h"

static u32 *kernel_page_dir;

/* ── Inline Assembly Helpers ─────────────────────────────────────────────── */
static inline void load_page_directory(u32 addr) {
    __asm__ volatile("mov %0, %%cr3" : : "r"(addr));
}

static inline void enable_paging(void) {
    u32 cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; /* PG bit */
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

/* ── Public: Initialisation ──────────────────────────────────────────────── */
void paging_init(void) {
    /* Step 1: Allocate Page Directory */
    kernel_page_dir = (u32 *)pmm_alloc_frame();
    
    /* Zero out the directory */
    for (int i = 0; i < 1024; i++) {
        kernel_page_dir[i] = 0x00000002; /* Not present, writable */
    }

    /* Step 2: Create a page table to identity map the first 4MB */
    u32 *first_page_table = (u32 *)pmm_alloc_frame();

    for (u32 i = 0; i < 1024; i++) {
        /* Physical address i * 4096, Present, Writable */
        first_page_table[i] = (i * 4096) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    /* Put the page table into the directory */
    kernel_page_dir[0] = (u32)first_page_table | PAGE_PRESENT | PAGE_WRITABLE;

    /* Step 3: Enable paging */
    load_page_directory((u32)kernel_page_dir);
    enable_paging();

    kprintf("  Paging: Initialised. Identity mapped first 4MB.\n");
    kprintf("  Paging: Directory at %p\n", kernel_page_dir);
}

/* ── Public: Mapping ─────────────────────────────────────────────────────── */
void paging_map(u32 virt, u32 phys, u32 flags) {
    u32 pd_idx = virt >> 22;
    u32 pt_idx = (virt >> 12) & 0x03FF;

    if (!(kernel_page_dir[pd_idx] & PAGE_PRESENT)) {
        /* Page table not present, allocate one */
        u32 *new_pt = (u32 *)pmm_alloc_frame();
        for (int i = 0; i < 1024; i++) new_pt[i] = 0;
        kernel_page_dir[pd_idx] = (u32)new_pt | PAGE_PRESENT | PAGE_WRITABLE;
    }

    u32 *pt = (u32 *)(kernel_page_dir[pd_idx] & 0xFFFFF000);
    pt[pt_idx] = (phys & 0xFFFFF000) | flags;

    /* Invalidate TLB for this address */
    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");
}
