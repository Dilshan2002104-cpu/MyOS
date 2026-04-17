

#include "keyboard.h"
#include "vga.h"
#include "../cpu/irq.h"
#include "../include/io.h"
#include "../kernel/kprintf.h"

static const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	
  '9', '0', '-', '=', '\b',	
  '\t',			
  'q', 'w', 'e', 'r',	
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	
    0,			
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	
 '\'', '`',   0,		
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			
  'm', ',', '.', '/',   0,				
  '*',
    0,	
  ' ',	
    0,	
    0,	
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	
    0,	
    0,	
    0,	
    0,	
    0,	
  '-',
    0,	
    0,
    0,	
  '+',
    0,	
    0,	
    0,	
    0,	
    0,	
    0,   0,   0,
    0,	
    0,	
    0,	
};

static const char sc_shifted[] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0
};

static bool shift_pressed = false;
static bool caps_lock = false;

#define BUFFER_SIZE 256
static char kbd_buffer[BUFFER_SIZE];
static u32 kbd_head = 0;
static u32 kbd_tail = 0;

static void buffer_put(char c) {
    u32 next = (kbd_head + 1) % BUFFER_SIZE;
    if (next != kbd_tail) {
        kbd_buffer[kbd_head] = c;
        kbd_head = next;
    }
}

static void keyboard_callback(registers_t *regs) {
    (void)regs;
    u8 scancode = inb(0x60);

    if (scancode & 0x80) {
        u8 released = scancode & 0x7F;
        if (released == 0x2A || released == 0x36) shift_pressed = false;
        return;
    }

    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return;
    }
    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return;
    }

    if (scancode < 128) {
        char c = 0;
        if (shift_pressed) {
            c = (scancode < sizeof(sc_shifted)) ? sc_shifted[scancode] : 0;
        } else {
            c = scancode_to_ascii[scancode];
        }

        if (caps_lock && c >= 'a' && c <= 'z') c -= 32;
        else if (caps_lock && c >= 'A' && c <= 'Z') c += 32;

        if (c > 0) {
            buffer_put(c);
        }
    }
}

void keyboard_init(void) {
    irq_register_handler(IRQ_KEYBOARD, keyboard_callback);
}

bool keyboard_has_char(void) {
    return kbd_head != kbd_tail;
}

char getchar(void) {
    while (!keyboard_has_char()) {
        __asm__ volatile("hlt");
    }
    char c = kbd_buffer[kbd_tail];
    kbd_tail = (kbd_tail + 1) % BUFFER_SIZE;
    return c;
}

void gets(char *buffer, u32 size) {
    u32 i = 0;
    while (i < size - 1) {
        char c = getchar();
        
        if (c == '\n') {
            vga_putchar('\n');
            break;
        }
        
        if (c == '\b') {
            if (i > 0) {
                i--;
                vga_putchar('\b'); 
                vga_putchar(' ');  
                vga_putchar('\b'); 
            }
            continue;
        }

        buffer[i++] = c;
        vga_putchar(c);
    }
    buffer[i] = '\0';
}
