/*
 * kernel/kernel.c — MyOS kernel entry point
 *
 * Phase 1: Demonstrates the VGA terminal driver and kprintf.
 */

#include "../drivers/vga.h"
#include "kprintf.h"

/* ─── Boot banner ────────────────────────────────────────────────────────── */
static void print_banner(void)
{
    vga_set_color(VGA_LIGHT_CYAN, VGA_BLACK);
    kputs("  __  __       ____  ____  \n");
    kputs(" |  \\/  |_   _/ __ \\/ ___| \n");
    kputs(" | |\\/| | | | | |  | |___  \n");
    kputs(" | |  | | |_| | |__| |___) |\n");
    kputs(" |_|  |_|\\__, |\\____/\\____/ \n");
    kputs("          |___/             \n");
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("  A bare-metal x86 OS built from scratch\n");
    kputs("\n");
}

/* ─── Divider line helper ─────────────────────────────────────────────────── */
static void print_divider(void)
{
    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("  ------------------------------------------------\n");
}

/* ─── Kernel entry point ─────────────────────────────────────────────────── */
void kernel_main(void)
{
    /* Initialise the VGA terminal (clears screen, enables cursor) */
    vga_init();

    print_banner();

    /* ── Phase status ─────────────────────────────────────────────────── */
    print_divider();

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 0 — Multiboot kernel boot  ");
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kputs("[ OK ]\n");

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 1 — VGA terminal + kprintf  ");
    vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
    kputs("[ OK ]\n");

    vga_set_color(VGA_DARK_GREY, VGA_BLACK);
    kputs("  Phase 2 -- GDT, IDT, interrupts    [ -- ]\n");
    kputs("  Phase 3 -- Memory management       [ -- ]\n");
    kputs("  Phase 4 -- Keyboard + timer        [ -- ]\n");
    kputs("  Phase 5 -- Virtual file system     [ -- ]\n");
    kputs("  Phase 6 -- User mode + ELF loader  [ -- ]\n");
    kputs("  Phase 7 -- System calls            [ -- ]\n");
    kputs("  Phase 8 -- Shell                   [ -- ]\n");

    print_divider();

    /* ── kprintf format-specifier demo ───────────────────────────────── */
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    kputs("\n  kprintf self-test\n");
    print_divider();

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);

    kprintf("  %%s  -> %s\n",  "Hello, kernel!");
    kprintf("  %%d  -> %d\n",  -42);
    kprintf("  %%u  -> %u\n",  3141592u);
    kprintf("  %%x  -> 0x%x\n", 0xDEADBEEFu);
    kprintf("  %%X  -> 0x%X\n", 0xCAFEBABEu);
    kprintf("  %%p  -> %p\n",   (void *)0xB8000);
    kprintf("  %%c  -> %c\n",  'K');

    print_divider();

    /* ── Colour palette demo ─────────────────────────────────────────── */
    vga_set_color(VGA_YELLOW, VGA_BLACK);
    kputs("\n  Colour palette\n");
    print_divider();

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    kputs("  ");

    vga_color_t palette[] = {
        VGA_BLACK,      VGA_BLUE,       VGA_GREEN,      VGA_CYAN,
        VGA_RED,        VGA_MAGENTA,    VGA_BROWN,      VGA_LIGHT_GREY,
        VGA_DARK_GREY,  VGA_LIGHT_BLUE, VGA_LIGHT_GREEN,VGA_LIGHT_CYAN,
        VGA_LIGHT_RED,  VGA_LIGHT_MAGENTA, VGA_YELLOW,  VGA_WHITE,
    };
    for (int i = 0; i < 16; i++) {
        vga_set_color(palette[i], VGA_BLACK);
        kprintf(" %X ", i);
    }

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    kputs("\n");
    print_divider();

    /* kernel halts here; boot.s spins in cli+hlt */
}
