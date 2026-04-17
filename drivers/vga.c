/* drivers/vga.c — VGA text-mode terminal driver
 *
 * Provides a full scrolling terminal over the VGA text buffer at 0xB8000.
 * Supports 80×25 characters, 16 foreground + 16 background colours,
 * hardware cursor control via I/O ports, and automatic scrolling.
 *
 * Phase 1.
 */

#include "vga.h"
#include "../include/io.h"

/* ── Hardware Constants ────────────────────────────────────────────────────── */
#define VGA_BUFFER    ((volatile u16 *)0xB8000)
#define VGA_COLS      80
#define VGA_ROWS      25

/* VGA CRT controller ports */
#define VGA_CTRL_PORT 0x3D4   /* index register  */
#define VGA_DATA_PORT 0x3D5   /* data  register  */
#define VGA_REG_CURSOR_HI 14
#define VGA_REG_CURSOR_LO 15

/* ── VGA cell helpers ─────────────────────────────────────────────────────── */
static inline u8  make_attr(vga_color_t fg, vga_color_t bg) { return (u8)((bg << 4) | (fg & 0x0F)); }
static inline u16 make_cell(char ch, u8 attr)               { return (u16)((u16)attr << 8 | (u8)ch); }

/* ── Terminal State ────────────────────────────────────────────────────────── */
static u32 term_col;
static u32 term_row;
static u8  term_attr;

/* ── Internal: move hardware cursor to match (term_col, term_row) ─────────── */
static void hw_cursor_sync(void)
{
    u16 pos = (u16)(term_row * VGA_COLS + term_col);
    outb(VGA_CTRL_PORT, VGA_REG_CURSOR_HI);
    outb(VGA_DATA_PORT, (u8)(pos >> 8));
    outb(VGA_CTRL_PORT, VGA_REG_CURSOR_LO);
    outb(VGA_DATA_PORT, (u8)(pos & 0xFF));
}

/* ── Internal: scroll the terminal up by one row ─────────────────────────── */
static void scroll(void)
{
    /* Move every row one position up */
    for (u32 row = 0; row < VGA_ROWS - 1; row++)
        for (u32 col = 0; col < VGA_COLS; col++)
            VGA_BUFFER[row * VGA_COLS + col] =
                VGA_BUFFER[(row + 1) * VGA_COLS + col];

    /* Blank the last row */
    for (u32 col = 0; col < VGA_COLS; col++)
        VGA_BUFFER[(VGA_ROWS - 1) * VGA_COLS + col] =
            make_cell(' ', term_attr);
}

/* ── Public: initialise the terminal ─────────────────────────────────────── */
void vga_init(void)
{
    term_col  = 0;
    term_row  = 0;
    term_attr = make_attr(VGA_LIGHT_GREY, VGA_BLACK); /* default: grey on black */
    vga_enable_cursor();
    vga_clear();
}

/* ── Public: clear the screen ─────────────────────────────────────────────── */
void vga_clear(void)
{
    for (u32 i = 0; i < VGA_ROWS * VGA_COLS; i++)
        VGA_BUFFER[i] = make_cell(' ', term_attr);
    term_col = 0;
    term_row = 0;
    hw_cursor_sync();
}

/* ── Public: set foreground + background colour ─────────────────────────── */
void vga_set_color(vga_color_t fg, vga_color_t bg)
{
    term_attr = make_attr(fg, bg);
}

/* ── Public: read current foreground + background colour ────────────────── */
void vga_get_color(vga_color_t *fg, vga_color_t *bg)
{
    if (fg) *fg = (vga_color_t)(term_attr & 0x0F);
    if (bg) *bg = (vga_color_t)(term_attr >> 4);
}

/* ── Public: move hardware cursor to (col, row) ──────────────────────────── */
void vga_set_cursor(u32 col, u32 row)
{
    if (col < VGA_COLS) term_col = col;
    if (row < VGA_ROWS) term_row = row;
    hw_cursor_sync();
}

/* ── Public: read current cursor position ────────────────────────────────── */
void vga_get_cursor(u32 *col, u32 *row)
{
    if (col) *col = term_col;
    if (row) *row = term_row;
}

/* ── Public: enable the hardware blinking cursor ─────────────────────────── */
void vga_enable_cursor(void)
{
    /* Set scan-line start=14, end=15 (underline cursor) */
    outb(VGA_CTRL_PORT, 0x0A);
    u8 val = inb(VGA_DATA_PORT);
    outb(VGA_DATA_PORT, (val & 0xC0) | 14);   /* clear CD bit, set start scanline */
    outb(VGA_CTRL_PORT, 0x0B);
    val = inb(VGA_DATA_PORT);
    outb(VGA_DATA_PORT, (val & 0xE0) | 15);   /* set end scanline */
}

/* ── Public: hide the hardware cursor ───────────────────────────────────── */
void vga_disable_cursor(void)
{
    outb(VGA_CTRL_PORT, 0x0A);
    outb(VGA_DATA_PORT, 0x20);  /* bit 5 = cursor disabled */
}

/* ── Public: write a single character (handles control codes) ───────────── */
void vga_putchar(char c)
{
    switch (c) {
    case '\n':                          /* newline */
        term_col = 0;
        term_row++;
        break;

    case '\r':                          /* carriage return */
        term_col = 0;
        break;

    case '\t':                          /* tab — align to next 8-col boundary */
        term_col = (term_col + 8) & ~7u;
        if (term_col >= VGA_COLS) {
            term_col = 0;
            term_row++;
        }
        break;

    case '\b':                          /* backspace */
        if (term_col > 0) {
            term_col--;
        } else if (term_row > 0) {
            term_row--;
            term_col = VGA_COLS - 1;
        }
        VGA_BUFFER[term_row * VGA_COLS + term_col] = make_cell(' ', term_attr);
        break;

    default:                            /* printable character */
        if ((u8)c < 0x20) break;        /* silently drop other control chars */
        VGA_BUFFER[term_row * VGA_COLS + term_col] = make_cell(c, term_attr);
        term_col++;
        break;
    }

    /* Wrap to next row on column overflow */
    if (term_col >= VGA_COLS) {
        term_col = 0;
        term_row++;
    }

    /* Scroll if we've run off the bottom */
    if (term_row >= VGA_ROWS) {
        scroll();
        term_row = VGA_ROWS - 1;
    }

    hw_cursor_sync();
}

/* ── Public: write a null-terminated string ─────────────────────────────── */
void vga_puts(const char *str)
{
    while (*str)
        vga_putchar(*str++);
}
