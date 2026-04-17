

#include "kprintf.h"
#include "../drivers/vga.h"
#include <stdarg.h>

void kputchar(char c)        { vga_putchar(c); }
void kputs(const char *str)  { vga_puts(str); }

static void print_uint(u32 val, u32 base, bool uppercase)
{
    const char *digits_lo = "0123456789abcdef";
    const char *digits_hi = "0123456789ABCDEF";
    const char *digits    = uppercase ? digits_hi : digits_lo;

    char buf[32];   
    int  i = 0;

    if (val == 0) {
        vga_putchar('0');
        return;
    }

    while (val > 0) {
        buf[i++] = digits[val % base];
        val /= base;
    }

    for (int j = i - 1; j >= 0; j--)
        vga_putchar(buf[j]);
}

static void print_int(i32 val)
{
    if (val < 0) {
        vga_putchar('-');
        
        print_uint((u32)(-(val + 1)) + 1u, 10, false);
    } else {
        print_uint((u32)val, 10, false);
    }
}

void kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {

        if (*p != '%') {
            vga_putchar(*p);
            continue;
        }

        p++;   

        switch (*p) {
        case 'c':
            vga_putchar((char)va_arg(args, int));
            break;

        case 's': {
            const char *str = va_arg(args, const char *);
            vga_puts(str ? str : "(null)");
            break;
        }

        case 'd':
            print_int(va_arg(args, i32));
            break;

        case 'u':
            print_uint(va_arg(args, u32), 10, false);
            break;

        case 'x':
            print_uint(va_arg(args, u32), 16, false);
            break;

        case 'X':
            print_uint(va_arg(args, u32), 16, true);
            break;

        case 'p':
            vga_puts("0x");
            print_uint((u32)(uintptr_t)va_arg(args, void *), 16, false);
            break;

        case '%':
            vga_putchar('%');
            break;

        case '\0':   
            p--;
            break;

        default:     
            vga_putchar('%');
            vga_putchar(*p);
            break;
        }
    }

    va_end(args);
}
