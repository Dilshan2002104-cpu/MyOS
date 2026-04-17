#pragma once

#include "../include/types.h"

typedef enum {
    VGA_BLACK         = 0x0,
    VGA_BLUE          = 0x1,
    VGA_GREEN         = 0x2,
    VGA_CYAN          = 0x3,
    VGA_RED           = 0x4,
    VGA_MAGENTA       = 0x5,
    VGA_BROWN         = 0x6,
    VGA_LIGHT_GREY    = 0x7,
    VGA_DARK_GREY     = 0x8,
    VGA_LIGHT_BLUE    = 0x9,
    VGA_LIGHT_GREEN   = 0xA,
    VGA_LIGHT_CYAN    = 0xB,
    VGA_LIGHT_RED     = 0xC,
    VGA_LIGHT_MAGENTA = 0xD,
    VGA_YELLOW        = 0xE,
    VGA_WHITE         = 0xF,
} vga_color_t;

void vga_init(void);
void vga_clear(void);
void vga_putchar(char c);
void vga_puts(const char *str);
void vga_set_color(vga_color_t fg, vga_color_t bg);
void vga_get_color(vga_color_t *fg, vga_color_t *bg);
void vga_set_cursor(u32 col, u32 row);
void vga_get_cursor(u32 *col, u32 *row);
void vga_enable_cursor(void);
void vga_disable_cursor(void);
