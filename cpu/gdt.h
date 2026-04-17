#pragma once
/* cpu/gdt.h — Global Descriptor Table (Phase 2) */

#include "../include/types.h"

/* A single GDT entry (segment descriptor) — 8 bytes                     */
/* Bit layout: see Intel SDM Vol 3, Section 3.4.5                         */
typedef struct {
    u16 limit_lo;       /* limit  bits 15:0                               */
    u16 base_lo;        /* base   bits 15:0                               */
    u8  base_mid;       /* base   bits 23:16                              */
    u8  access;         /* P | DPL[1:0] | S | Type[3:0]                  */
    u8  flags_lim_hi;   /* G | D/B | L | AVL | limit bits 19:16          */
    u8  base_hi;        /* base   bits 31:24                              */
} __attribute__((packed)) gdt_entry_t;

/* GDTR — the 6-byte value loaded by lgdt                                 */
typedef struct {
    u16 limit;          /* sizeof(gdt) - 1                                */
    u32 base;           /* linear address of first GDT entry              */
} __attribute__((packed)) gdt_ptr_t;

/* Segment selectors (index × 8, TI=0, RPL=0)                            */
#define GDT_SEL_NULL   0x00
#define GDT_SEL_KCODE  0x08   /* Entry 1: kernel code, ring 0            */
#define GDT_SEL_KDATA  0x10   /* Entry 2: kernel data, ring 0            */

void gdt_init(void);
