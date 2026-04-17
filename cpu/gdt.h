#pragma once

#include "../include/types.h"

typedef struct {
    u16 limit_lo;       
    u16 base_lo;        
    u8  base_mid;       
    u8  access;         
    u8  flags_lim_hi;   
    u8  base_hi;        
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    u16 limit;          
    u32 base;           
} __attribute__((packed)) gdt_ptr_t;

#define GDT_SEL_NULL   0x00
#define GDT_SEL_KCODE  0x08   
#define GDT_SEL_KDATA  0x10   

void gdt_init(void);
