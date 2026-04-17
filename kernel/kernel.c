

#include "../drivers/vga.h"
#include "../drivers/pit.h"
#include "../drivers/keyboard.h"
#include "../cpu/gdt.h"
#include "../cpu/idt.h"
#include "../cpu/irq.h"
#include "../mm/pmm.h"
#include "../mm/paging.h"
#include "../mm/heap.h"
#include "../fs/vfs.h"
#include "../fs/initrd.h"
#include "../include/io.h"
#include "../include/multiboot.h"
#include "../include/string.h"
#include "kprintf.h"

extern char __kernel_end[];

static multiboot_info_t mbi_safe;

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

void kernel_main(u32 magic, u32 addr) {
    vga_init();
    print_banner();
    print_div();

    memcpy(&mbi_safe, (void*)addr, sizeof(multiboot_info_t));
    multiboot_info_t *mbi = &mbi_safe;

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kprintf("  Phase 0-2 -- CPU: Magic=0x%x Flags=0x%x ", magic, mbi->flags);
    gdt_init();
    idt_init();
    __asm__ volatile("sti");
    ok();

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kprintf("  Phase 3 -- Memory: mmap=0x%x mods=0x%x ", mbi->mmap_addr, mbi->mods_addr);

    u32 real_kernel_end = (u32)__kernel_end;

    if (mbi->flags & (1 << 3)) {
        multiboot_module_t *mod = (multiboot_module_t *)mbi->mods_addr;
        for (u32 i = 0; i < mbi->mods_count; i++) {
            if (mod[i].mod_end > real_kernel_end) real_kernel_end = mod[i].mod_end;
        }
        
        u32 mods_array_end = mbi->mods_addr + mbi->mods_count * sizeof(multiboot_module_t);
        if (mods_array_end > real_kernel_end) real_kernel_end = mods_array_end;
    }

    pmm_init(mbi->mmap_addr, mbi->mmap_length, real_kernel_end);

    pmm_mark_used(addr, sizeof(multiboot_info_t));
    if (mbi->flags & (1 << 3)) {
        multiboot_module_t *mod = (multiboot_module_t *)mbi->mods_addr;
        pmm_mark_used(mbi->mods_addr, mbi->mods_count * sizeof(multiboot_module_t));
        for (u32 i = 0; i < mbi->mods_count; i++) {
            pmm_mark_used(mod[i].mod_start, mod[i].mod_end - mod[i].mod_start);
        }
    }

    paging_init(); 
    paging_enable(paging_get_kernel_pd());

    heap_init(0xC0000000, 1024 * 1024);
    ok();

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 4 -- Keyboard + timer       ");
    pit_init(1000);
    keyboard_init();
    ok();

    vga_set_color(VGA_WHITE, VGA_BLACK);
    kputs("  Phase 5 -- Virtual file system    ");
    
    if (mbi->flags & (1 << 3) && mbi->mods_count > 0) {
        multiboot_module_t *mod = (multiboot_module_t *)mbi->mods_addr;
        kprintf("\n    - Initrd module found at %p (size %u bytes)\n", 
                mod->mod_start, mod->mod_end - mod->mod_start);
        
        fs_root = initrd_init(mod->mod_start);
        if (fs_root) ok();
        else kputs("[FAIL: initrd_init]\n");
    } else {
        kputs("[FAIL: No modules]\n");
    }

    print_div();
    if (fs_root) {
        kprintf("  Phase 5 Demo: Listing / [root]\n");
        int i = 0;
        struct dirent *node = NULL;
        while ((node = vfs_readdir(fs_root, i)) != 0) {
            fs_node_t *fsnode = vfs_finddir(fs_root, node->name);
            kprintf("    > %s (%u bytes)\n", node->name, fsnode ? fsnode->length : 0);
            i++;
        }

        fs_node_t *test_file = vfs_finddir(fs_root, "test.txt");
        if (test_file) {
            char buffer[64];
            u32 size = vfs_read(test_file, 0, 63, (u8*)buffer);
            buffer[size] = '\0';
            vga_set_color(VGA_LIGHT_GREEN, VGA_BLACK);
            kprintf("  Content: \"%s\"\n", buffer);
        }
    }

    vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
    kputs("\n  System live. > ");

    while (1) {
        u32 col, row;
        vga_get_cursor(&col, &row);
        vga_set_cursor(58, 24);
        vga_set_color(VGA_DARK_GREY, VGA_BLACK);
        kprintf("uptime: %u s ", pit_get_ticks() / 1000);
        vga_set_color(VGA_LIGHT_GREY, VGA_BLACK);
        vga_set_cursor(col, row);

        if (keyboard_has_char()) {
            char c = getchar();
            if (c == '\n') kputs("\n  > ");
            else vga_putchar(c);
        }
        __asm__ volatile("hlt");
    }
}
