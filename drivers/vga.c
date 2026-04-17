

#include "vga.h"
#include "../include/io.h"

#define VGA_BUFFER    ((volatile u16 *)0xB8000)
#define VGA_COLS      80
#define VGA_ROWS      25

#define VGA_CTRL_PORT 0x3D4   
#define VGA_DATA_PORT 0x3D5   
#define VGA_REG_CURSOR_HI 14
#define VGA_REG_CURSOR_LO 15

static inline u8  make_attr(vga_color_t fg, vga_color_t bg) { return (u8)((bg << 4) | (fg & 0x0F)); }
static inline u16 make_cell(char ch, u8 attr)               { return (u16)((u16)attr << 8 | (u8)ch); }

static u32 term_col;
static u32 term_row;
static u8  term_attr;

static void hw_cursor_sync(void)
{
    u16 pos = (u16)(term_row * VGA_COLS + term_col);
    outb(VGA_CTRL_PORT, VGA_REG_CURSOR_HI);
    outb(VGA_DATA_PORT, (u8)(pos >> 8));
    outb(VGA_CTRL_PORT, VGA_REG_CURSOR_LO);
    outb(VGA_DATA_PORT, (u8)(pos & 0xFF));
}

static void scroll(void)
{
    
    for (u32 row = 0; row < VGA_ROWS - 1; row++)
        for (u32 col = 0; col < VGA_COLS; col++)
            VGA_BUFFER[row * VGA_COLS + col] =
                VGA_BUFFER[(row + 1) * VGA_COLS + col];

    for (u32 col = 0; col < VGA_COLS; col++)
        VGA_BUFFER[(VGA_ROWS - 1) * VGA_COLS + col] =
            make_cell(' ', term_attr);
}

void vga_init(void)
{
    term_col  = 0;
    term_row  = 0;
    term_attr = make_attr(VGA_LIGHT_GREY, VGA_BLACK); 
    vga_enable_cursor();
    vga_clear();
}

void vga_clear(void)
{
    for (u32 i = 0; i < VGA_ROWS * VGA_COLS; i++)
        VGA_BUFFER[i] = make_cell(' ', term_attr);
    term_col = 0;
    term_row = 0;
    hw_cursor_sync();
}

void vga_set_color(vga_color_t fg, vga_color_t bg)
{
    term_attr = make_attr(fg, bg);
}

void vga_get_color(vga_color_t *fg, vga_color_t *bg)
{
    if (fg) *fg = (vga_color_t)(term_attr & 0x0F);
    if (bg) *bg = (vga_color_t)(term_attr >> 4);
}

void vga_set_cursor(u32 col, u32 row)
{
    if (col < VGA_COLS) term_col = col;
    if (row < VGA_ROWS) term_row = row;
    hw_cursor_sync();
}

void vga_get_cursor(u32 *col, u32 *row)
{
    if (col) *col = term_col;
    if (row) *row = term_row;
}

void vga_enable_cursor(void)
{
    
    outb(VGA_CTRL_PORT, 0x0A);
    u8 val = inb(VGA_DATA_PORT);
    outb(VGA_DATA_PORT, (val & 0xC0) | 14);   
    outb(VGA_CTRL_PORT, 0x0B);
    val = inb(VGA_DATA_PORT);
    outb(VGA_DATA_PORT, (val & 0xE0) | 15);   
}

void vga_disable_cursor(void)
{
    outb(VGA_CTRL_PORT, 0x0A);
    outb(VGA_DATA_PORT, 0x20);  
}

void vga_putchar(char c)
{
    switch (c) {
    case '\n':                          
        term_col = 0;
        term_row++;
        break;

    case '\r':                          
        term_col = 0;
        break;

    case '\t':                          
        term_col = (term_col + 8) & ~7u;
        if (term_col >= VGA_COLS) {
            term_col = 0;
            term_row++;
        }
        break;

    case '\b':                          
        if (term_col > 0) {
            term_col--;
        } else if (term_row > 0) {
            term_row--;
            term_col = VGA_COLS - 1;
        }
        VGA_BUFFER[term_row * VGA_COLS + term_col] = make_cell(' ', term_attr);
        break;

    default:                            
        if ((u8)c < 0x20) break;        
        VGA_BUFFER[term_row * VGA_COLS + term_col] = make_cell(c, term_attr);
        term_col++;
        break;
    }

    if (term_col >= VGA_COLS) {
        term_col = 0;
        term_row++;
    }

    if (term_row >= VGA_ROWS) {
        scroll();
        term_row = VGA_ROWS - 1;
    }

    hw_cursor_sync();
}

void vga_puts(const char *str)
{
    while (*str)
        vga_putchar(*str++);
}
