

#include "heap.h"
#include "paging.h"
#include "pmm.h"
#include "../kernel/kprintf.h"

#define HEAP_MAGIC 0xC0FFEE

typedef struct heap_header {
    u32 magic;
    u32 size;       
    bool is_free;
    struct heap_header *next;
} heap_header_t;

static heap_header_t *heap_start = NULL;

void heap_init(u32 start_addr, u32 size) {
    
    u32 start = PAGE_UP(start_addr);
    u32 end   = PAGE_ALIGN(start_addr + size);
    u32 real_size = end - start;

    for (u32 addr = start; addr < end; addr += PAGE_SIZE) {
        paging_map(addr, pmm_alloc_frame(), PAGE_PRESENT | PAGE_WRITABLE);
    }

    heap_start = (heap_header_t *)start;
    heap_start->magic = HEAP_MAGIC;
    heap_start->size  = real_size - sizeof(heap_header_t);
    heap_start->is_free = true;
    heap_start->next = NULL;

    kprintf("  Heap: Initialised at %p, size %u KB\n", start, real_size / 1024);
}

void *kmalloc(u32 size) {
    heap_header_t *curr = heap_start;

    while (curr) {
        if (curr->is_free && curr->size >= size) {
            
            if (curr->size >= size + sizeof(heap_header_t) + 16) {
                heap_header_t *next = (heap_header_t *)((u32)curr + sizeof(heap_header_t) + size);
                next->magic = HEAP_MAGIC;
                next->size = curr->size - size - sizeof(heap_header_t);
                next->is_free = true;
                next->next = curr->next;

                curr->size = size;
                curr->next = next;
            }

            curr->is_free = false;
            return (void *)((u32)curr + sizeof(heap_header_t));
        }
        curr = curr->next;
    }

    kprintf("  Heap: OUT OF MEMORY trying to allocate %u bytes\n", size);
    return NULL;
}

void *kmalloc_aligned(u32 size) {
    
    void *ptr = kmalloc(size + PAGE_SIZE);
    u32 addr = (u32)ptr;
    return (void *)PAGE_UP(addr);
}

void kfree(void *ptr) {
    if (!ptr) return;

    heap_header_t *header = (heap_header_t *)((u32)ptr - sizeof(heap_header_t));
    if (header->magic != HEAP_MAGIC) {
        kprintf("  Heap: kfree ERROR - Invalid magic!\n");
        return;
    }

    header->is_free = true;

    if (header->next && header->next->is_free) {
        header->size += header->next->size + sizeof(heap_header_t);
        header->next = header->next->next;
    }

}

void heap_dump(void) {
    heap_header_t *curr = heap_start;
    kprintf("  --- Heap Dump ---\n");
    while (curr) {
        kprintf("  Block at %p: size=%u, free=%d\n", curr, curr->size, curr->is_free);
        curr = curr->next;
    }
}
