/* mm/pmm.c — Physical Memory Manager
 *
 * Tracks 4KB physical page frames using a bitmap.
 * Initialised using the Multiboot memory map.
 *
 * Phase 3.
 */

#include "pmm.h"
#include "../include/multiboot.h"
#include "../kernel/kprintf.h"

/* ── Global State ────────────────────────────────────────────────────────── */
static u32 *pmm_bitmap;
static u32  pmm_max_frames;
static u32  pmm_used_frames;

/* ── Bitmap Helpers ──────────────────────────────────────────────────────── */
static inline void bitmap_set(u32 bit) {
    pmm_bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_clear(u32 bit) {
    pmm_bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline bool bitmap_test(u32 bit) {
    return pmm_bitmap[bit / 32] & (1 << (bit % 32));
}

/* Find first free block of 32 bits and then the first bit */
static i32 bitmap_first_free(void) {
    for (u32 i = 0; i < pmm_max_frames / 32; i++) {
        if (pmm_bitmap[i] != 0xFFFFFFFF) {
            for (int j = 0; j < 32; j++) {
                if (!(pmm_bitmap[i] & (1 << j))) {
                    return (i * 32) + j;
                }
            }
        }
    }
    return -1;
}

/* ── Public: Initialise ──────────────────────────────────────────────────── */
void pmm_init(u32 mmap_addr, u32 mmap_len, u32 kernel_end) {
    /* Step 1: Find total memory by looking at mmap entries */
    u64 total_mem = 0;
    multiboot_memory_map_t *mmap = (multiboot_memory_map_t *)mmap_addr;

    while ((u32)mmap < mmap_addr + mmap_len) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            if (mmap->addr + mmap->len > total_mem) {
                total_mem = mmap->addr + mmap->len;
            }
        }
        mmap = (multiboot_memory_map_t *)((u32)mmap + mmap->size + sizeof(u32));
    }

    pmm_max_frames = (u32)(total_mem / PAGE_SIZE);
    pmm_used_frames = pmm_max_frames;

    /* Step 2: Place bitmap right after the kernel */
    pmm_bitmap = (u32 *)PAGE_UP(kernel_end);
    u32 bitmap_size = pmm_max_frames / 8;
    
    /* Mark everything as used/reserved initially */
    for (u32 i = 0; i < pmm_max_frames / 32; i++) {
        pmm_bitmap[i] = 0xFFFFFFFF;
    }

    /* Step 3: Parse mmap again and mark available regions as free */
    mmap = (multiboot_memory_map_t *)mmap_addr;
    while ((u32)mmap < mmap_addr + mmap_len) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            pmm_mark_free((u32)mmap->addr, (u32)mmap->len);
        }
        mmap = (multiboot_memory_map_t *)((u32)mmap + mmap->size + sizeof(u32));
    }

    /* Step 4: Protect critical regions (mark as used) */
    /* First 1MB (BIOS, VGA, etc.) */
    pmm_mark_used(0x0, 0x100000);
    /* Kernel + PMM Bitmap */
    pmm_mark_used(0x100000, kernel_end - 0x100000 + bitmap_size);

    kprintf("  PMM: Initialised with %u MB total memory\n", (u32)(total_mem / 1024 / 1024));
    kprintf("  PMM: Bitmap located at %p, size %u KB\n", pmm_bitmap, bitmap_size / 1024);
    kprintf("  PMM: Free frames: %u / %u\n", pmm_get_free_frames(), pmm_max_frames);
}

/* ── Public: Allocation ──────────────────────────────────────────────────── */
u32 pmm_alloc_frame(void) {
    i32 frame = bitmap_first_free();
    if (frame == -1) return 0; /* Out of memory */

    bitmap_set((u32)frame);
    pmm_used_frames++;
    return (u32)frame * PAGE_SIZE;
}

void pmm_free_frame(u32 frame_addr) {
    u32 frame = frame_addr / PAGE_SIZE;
    if (bitmap_test(frame)) {
        bitmap_clear(frame);
        pmm_used_frames--;
    }
}

/* ── Public: Utilities ───────────────────────────────────────────────────── */
u32 pmm_get_free_frames(void)  { return pmm_max_frames - pmm_used_frames; }
u32 pmm_get_total_frames(void) { return pmm_max_frames; }

void pmm_mark_used(u32 start, u32 len) {
    u32 frame = start / PAGE_SIZE;
    u32 count = PAGE_UP(len) / PAGE_SIZE;
    for (u32 i = 0; i < count; i++) {
        if (!bitmap_test(frame + i)) {
            bitmap_set(frame + i);
            pmm_used_frames++;
        }
    }
}

void pmm_mark_free(u32 start, u32 len) {
    u32 frame = start / PAGE_SIZE;
    u32 count = len / PAGE_SIZE; /* Be conservative with free regions */
    for (u32 i = 0; i < count; i++) {
        if (bitmap_test(frame + i)) {
            bitmap_clear(frame + i);
            pmm_used_frames--;
        }
    }
}
