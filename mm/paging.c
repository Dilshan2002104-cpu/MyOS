

#include "paging.h"
#include "pmm.h"
#include "../kernel/kprintf.h"

static u32 *kernel_page_dir;

static inline void load_page_directory(u32 addr) {
    __asm__ volatile("mov %0, %%cr3" : : "r"(addr));
}

void paging_init(void) {
    
    kernel_page_dir = (u32 *)pmm_alloc_frame();

    for (int i = 0; i < 1024; i++) {
        kernel_page_dir[i] = 0x00000002; 
    }

    u32 *first_pt = (u32 *)pmm_alloc_frame();
    u32 *second_pt = (u32 *)pmm_alloc_frame();

    for (u32 i = 0; i < 1024; i++) {
        first_pt[i] = (i * 4096) | PAGE_PRESENT | PAGE_WRITABLE;
        second_pt[i] = (1024 * 4096 + i * 4096) | PAGE_PRESENT | PAGE_WRITABLE;
    }

    kernel_page_dir[0] = (u32)first_pt | PAGE_PRESENT | PAGE_WRITABLE;
    kernel_page_dir[1] = (u32)second_pt | PAGE_PRESENT | PAGE_WRITABLE;

    kprintf("  Paging: Structures prepared (Directory at %p)\n", kernel_page_dir);
}

void paging_enable(u32 pd_addr) {
    load_page_directory(pd_addr);
    u32 cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; 
    __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
    kprintf("  Paging: Enabled.\n");
}

u32 paging_get_kernel_pd(void) {
    return (u32)kernel_page_dir;
}

void paging_map(u32 virt, u32 phys, u32 flags) {
    u32 pd_idx = virt >> 22;
    u32 pt_idx = (virt >> 12) & 0x03FF;

    if (!(kernel_page_dir[pd_idx] & PAGE_PRESENT)) {
        
        u32 *new_pt = (u32 *)pmm_alloc_frame();
        for (int i = 0; i < 1024; i++) new_pt[i] = 0;
        kernel_page_dir[pd_idx] = (u32)new_pt | PAGE_PRESENT | PAGE_WRITABLE;
    }

    u32 *pt = (u32 *)(kernel_page_dir[pd_idx] & 0xFFFFF000);
    pt[pt_idx] = (phys & 0xFFFFF000) | flags;

    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");
}
