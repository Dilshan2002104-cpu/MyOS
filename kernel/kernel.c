/*
 * kernel/kernel.c — MyOS kernel entry point
 *
 * Phase 3: Memory Management (PMM, Paging, Heap).
 */

#include "../drivers/vga.h"
#include "../cpu/gdt.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../cpu/irq.h"
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "../mm/heap.h"
#include "../include/io.h"
#include "../include/multiboot.h"
#include "kprintf.h"

/* Symbol from linker script */
extern char __kernel_end[];

/* ── Live timer state ────────────────────────────────────────────────────── */
static volatile u32 tick_count = 0;

static void timer_handler(registers_t *regs) {
    (void)regs;
    tick_count++;
    u32 col, row;
    vga_get_cursor(&col, &row);
    vga_set_cursor(58, 24);
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kprintf("uptime: %u ticks", tick_count);
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_set_cursor(col, row);
}

/* ── Keyboard state ──────────────────────────────────────────────────────── */
static u32 kb_col = 2;
static void keyboard_handler(registers_t *regs) {
    (void)regs;
    u8 scancode = inb(0x60);
    if (scancode & 0x80) return;
    u32 col, row;
    vga_get_cursor(&col, &row);
    vga_set_cursor(kb_col, 22);
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    kprintf("0x%x ", scancode);
    kb_col += 5;
    if (kb_col > 75) kb_col = 2;
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_set_cursor(col, row);
}

/* ── UI Helpers ─────────────────────────────────────────────────────────── */
static void print_banner(void) {
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    kputs("  __  __       ____  ____\n");
    kputs(" |  \\/  |_   _/ __ \\/ ___|\n");
    kputs(" | |\\/| | | | | |  | |___\n");
    kputs(" | |  | | |_| | |__| |___)\n");
    kputs(" |_|  |_|\\__, |\\____/\\____/\n");
    kputs("          |___/\n");
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("  A bare-metal x86 OS -- built from scratch\n\n");
}

static void print_div(void) {
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("  ------------------------------------------------\n");
}

static void ok(void) {
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kputs("[ OK ]\n");
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
}

static void pending(void) {
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("[ -- ]\n");
}

/* ── Kernel entry point ───────────────────────────────────────────────────── */
void kernel_main(u32 magic, u32 addr) {
    vga_init();
    print_banner();
    print_div();

    /* Phase 0 */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 0 -- Multiboot kernel       "); ok();

    /* Phase 1 */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 1 -- VGA terminal + kprintf "); ok();

    /* Phase 2 */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 2 -- GDT IDT IRQs           ");
    gdt_init();
    idt_init();
    irq_register_handler(IRQ_TIMER, timer_handler);
    irq_register_handler(IRQ_KEYBOARD, keyboard_handler);
    __asm__ volatile("sti");
    ok();

    /* Phase 3 ─────────────────────────────────────────────────────────── */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 3 -- Memory management      ");
    
    /* Multiboot check */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        vga_set_color(VGA_WHITE, VGA_RED);
        kprintf("PANIC: Invalid Multiboot magic 0x%x\n", magic);
        while(1);
    }

    multiboot_info_t *mbi = (multiboot_info_t *)addr;
    
    /* 1. Physical Memory Manager */
    pmm_init(mbi->mmap_addr, mbi->mmap_length, (u32)__kernel_end);

    /* 2. Paging */
    paging_init();

    /* 3. Heap (Allocate 1MB for kernel heap) */
    heap_init(0xC0000000, 1024 * 1024);
    
    ok();

    /* Remaining phases */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 4 -- Keyboard + timer       "); pending();
    kputs("  Phase 5 -- Virtual file system    "); pending();
    kputs("  Phase 6 -- User mode + ELF loader "); pending();
    kputs("  Phase 7 -- System calls           "); pending();
    kputs("  Phase 8 -- Shell                  "); pending();
    print_div();

    /* Phase 3 Demo: Dynamic Memory Allocation */
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    kputs("\n  Phase 3 Demo: Heap Allocation\n");
    print_div();

    void *ptr1 = kmalloc(128);
    void *ptr2 = kmalloc(256);
    kprintf("  Allocated: ptr1 @ %p (128 bytes), ptr2 @ %p (256 bytes)\n", ptr1, ptr2);
    
    kfree(ptr1);
    kprintf("  Freed ptr1. Allocated ptr3 (64 bytes):\n");
    void *ptr3 = kmalloc(64);
    kprintf("  ptr3 @ %p (should be near old ptr1)\n", ptr3);
    
    print_div();
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    kputs("  Memory management is live. Uptime ticking at bottom right.\n");

    while (1) {
        __asm__ volatile("hlt");
    }
}
