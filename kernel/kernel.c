/*
 * kernel.c — Minimal x86 bare-bones kernel
 *
 * Compiled with: gcc -m32 -ffreestanding -fno-stack-protector
 * No standard library is available in this environment.
 */

#include <stdint.h>
#include <stddef.h>

/* ── VGA Text-Mode Constants ───────────────────────────────────────────────── */
#define VGA_ADDRESS   ((volatile uint16_t *)0xB8000)
#define VGA_COLS      80
#define VGA_ROWS      25

/*
 * A VGA text-mode cell is 16 bits:
 *   Bits 15-8  : attribute byte  (background colour | foreground colour)
 *   Bits  7-0  : ASCII character
 *
 * Colour byte layout (each nibble):
 *   High nibble — background   Low nibble — foreground
 *   0x0 = Black   0x1 = Blue   0x2 = Green  0x3 = Cyan
 *   0x4 = Red     0x5 = Magenta 0x6 = Brown  0x7 = Light Grey
 *   0x8-0xF = bright variants
 */
#define VGA_COLOR(bg, fg)   (((bg) << 4) | (fg))
#define VGA_ENTRY(ch, attr) ((uint16_t)(attr) << 8 | (uint8_t)(ch))

/* Palette shortcuts */
#define COLOR_BLACK        0x0
#define COLOR_LIGHT_GREEN  0xA
#define COLOR_WHITE        0xF

/* ── Helper: Write a single character at (col, row) ─────────────────────────── */
static inline void
vga_putchar(size_t col, size_t row, char ch, uint8_t attr)
{
    VGA_ADDRESS[row * VGA_COLS + col] = VGA_ENTRY(ch, attr);
}

/* ── Clear the entire VGA screen ─────────────────────────────────────────────── */
static void
vga_clear(uint8_t attr)
{
    for (size_t row = 0; row < VGA_ROWS; row++)
        for (size_t col = 0; col < VGA_COLS; col++)
            vga_putchar(col, row, ' ', attr);
}

/* ── Print a null-terminated string starting at (col, row) ──────────────────── */
static void
vga_print(size_t col, size_t row, const char *str, uint8_t attr)
{
    for (size_t i = 0; str[i] != '\0'; i++)
        vga_putchar(col + i, row, str[i], attr);
}

/* ── Kernel Entry Point ──────────────────────────────────────────────────────── */
void kernel_main(void)
{
    /* Dark blue background, white text for the whole screen */
    uint8_t bg_attr  = VGA_COLOR(COLOR_BLACK, COLOR_WHITE);

    /* Bright green text for the greeting — stands out nicely */
    uint8_t msg_attr = VGA_COLOR(COLOR_BLACK, COLOR_LIGHT_GREEN);

    vga_clear(bg_attr);

    /* Print greeting centred on the top row */
    vga_print(36, 12, "Hello OS!", msg_attr);

    /* Kernel halts; the infinite-loop / hlt is in boot.s */
}
