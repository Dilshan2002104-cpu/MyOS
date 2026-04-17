/*
 * kernel/kernel.c — MyOS kernel entry point
 *
 * Phase 2: GDT, IDT, CPU exception handlers, hardware IRQs.
 *          Timer tick counter + keyboard scancode display.
 */

#include "../drivers/vga.h"
#include "../cpu/gdt.h"
#include "../cpu/idt.h"
#include "../cpu/isr.h"
#include "../cpu/irq.h"
#include "../include/io.h"
#include "kprintf.h"

/* ── Live timer state (updated by IRQ 0 handler) ─────────────────────────── */
static volatile u32 tick_count = 0;

/* ── IRQ 0: Timer ────────────────────────────────────────────────────────── */
static void timer_handler(registers_t *regs)
{
    (void)regs;
    tick_count++;

    /* Update tick counter at a fixed position (col 60, row 24) */
    u32 col, row;
    vga_get_cursor(&col, &row);

    vga_set_cursor(58, 24);
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kprintf("uptime: ");
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    kprintf("%u ticks  ", tick_count);

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_set_cursor(col, row);
}

/* ── IRQ 1: Keyboard ─────────────────────────────────────────────────────── */
static u32 kb_col = 2;   /* current column in the keyboard test row */

static void keyboard_handler(registers_t *regs)
{
    (void)regs;
    u8 scancode = inb(0x60);   /* read scancode from PS/2 data port */

    /* Only print on key-press (bit 7 = 0); ignore key-release (bit 7 = 1) */
    if (scancode & 0x80) return;

    u32 col, row;
    vga_get_cursor(&col, &row);

    /* Print scancode in hex at row 22 */
    vga_set_cursor(kb_col, 22);
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    kprintf("0x%x ", scancode);
    kb_col += 5;
    if (kb_col > 75) kb_col = 2;   /* wrap */

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    vga_set_cursor(col, row);
}

/* ── Boot banner ──────────────────────────────────────────────────────────── */
static void print_banner(void)
{
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

static void print_div(void)
{
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("  ------------------------------------------------\n");
}

static void ok(void)
{
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kputs("[ OK ]\n");
    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
}

static void pending(void)
{
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("[ -- ]\n");
}

/* ── Kernel entry point ───────────────────────────────────────────────────── */
void kernel_main(void)
{
    /* Phase 0 implied: we reached here thanks to boot.s */
    vga_init();
    print_banner();
    print_div();

    /* Phase 0 */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 0 -- Multiboot kernel       "); ok();

    /* Phase 1 */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 1 -- VGA terminal + kprintf "); ok();

    /* Phase 2 ─────────────────────────── */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 2 -- GDT ");

    gdt_init();
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kputs("[GDT] ");

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("IDT ");

    /*
     * idt_init() calls isr_init() (CPU exceptions 0-31)
     *              and irq_init() (PIC remapping + IRQs 0-15)
     */
    idt_init();
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kputs("[IDT] ");

    /* Register our live-demo IRQ handlers BEFORE enabling interrupts */
    irq_register_handler(IRQ_TIMER,    timer_handler);
    irq_register_handler(IRQ_KEYBOARD, keyboard_handler);

    /* Enable hardware interrupts */
    __asm__ volatile("sti");

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("IRQs "); ok();

    /* Remaining phases */
    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 3 -- Memory management      "); pending();
    kputs("  Phase 4 -- Keyboard + timer       "); pending();
    kputs("  Phase 5 -- Virtual file system    "); pending();
    kputs("  Phase 6 -- User mode + ELF loader "); pending();
    kputs("  Phase 7 -- System calls           "); pending();
    kputs("  Phase 8 -- Shell                  "); pending();

    print_div();

    /* ── Phase 2 interactive demo ───────────────────────────────────────── */
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    kputs("\n  Phase 2 Demo\n");
    print_div();

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    kputs("  Interrupts enabled (sti). Timer + keyboard IRQs active.\n");
    kputs("  Press keys to see PS/2 scancodes:\n");
    kputs("  ");   /* indent for keyboard row (row 22) */

    /* ── Spin forever — IRQ handlers update the display ─────────────────── */
    while (1) {
        __asm__ volatile("hlt");   /* halt until next interrupt */
    }
}
