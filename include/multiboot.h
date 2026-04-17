#pragma once
/* include/multiboot.h — Multiboot 1 information structure definitions */

#include "types.h"

#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002

typedef struct multiboot_header {
    u32 magic;
    u32 flags;
    u32 checksum;
    u32 header_addr;
    u32 load_addr;
    u32 load_end_addr;
    u32 bss_end_addr;
    u32 entry_addr;
    u32 mode_type;
    u32 width;
    u32 height;
    u32 depth;
} multiboot_header_t;

typedef struct multiboot_mmap_entry {
    u32 size;
    u64 addr;
    u64 len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
    u32 type;
} __attribute__((packed)) multiboot_memory_map_t;

typedef struct multiboot_info {
    u32 flags;
    u32 mem_lower;
    u32 mem_upper;
    u32 boot_device;
    u32 cmdline;
    u32 mods_count;
    u32 mods_addr;
    u32 num;
    u32 size;
    u32 addr;
    u32 shndx;
    u32 mmap_length;
    u32 mmap_addr;
    u32 drives_length;
    u32 drives_addr;
    u32 config_table;
    u32 boot_loader_name;
    u32 apm_table;
    u32 vbe_control_info;
    u32 vbe_mode_info;
    u16 vbe_mode;
    u16 vbe_interface_seg;
    u16 vbe_interface_off;
    u16 vbe_interface_len;
} __attribute__((packed)) multiboot_info_t;

#define MULTIBOOT_INFO_MEM_MAP 0x00000040
