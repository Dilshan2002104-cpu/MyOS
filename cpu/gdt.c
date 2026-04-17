/* cpu/gdt.c — Global Descriptor Table setup
 *
 * Sets up a flat 32-bit memory model with three segments:
 *   0: Null      (required by CPU)
 *   1: Kernel code — ring 0, present, executable, readable, 4 GB
 *   2: Kernel data — ring 0, present, writable, 4 GB
 *
 * User-mode segments (ring 3) and TSS will be added in Phase 6.
 *
 * Phase 2.
 */

#include "gdt.h"

/* ── GDT storage ─────────────────────────────────────────────────────────── */
#define GDT_ENTRIES 3

static gdt_entry_t gdt[GDT_ENTRIES];
static gdt_ptr_t   gdt_ptr;

/* Declared in gdt_flush.s */
extern void gdt_flush(u32 gdt_ptr_addr);

/* ── Access byte constants ───────────────────────────────────────────────── */
/*   Bit 7  : Present
 *   Bits 6-5: DPL (00=ring0, 11=ring3)
 *   Bit 4  : S=1 (code/data descriptor)
 *   Bit 3  : Executable
 *   Bit 2  : Direction/Conforming
 *   Bit 1  : Readable (code) / Writable (data)
 *   Bit 0  : Accessed
 */
#define ACCESS_KCODE  0x9A   /* 1 00 1 1010 — kernel code, exec+read      */
#define ACCESS_KDATA  0x92   /* 1 00 1 0010 — kernel data, read+write     */

/* ── Flags nibble (upper 4 bits of flags_lim_hi) ─────────────────────── */
/*   G=1  (granularity: limit unit = 4 KiB pages)
 *   D=1  (32-bit protected mode)
 *   L=0  (not 64-bit)
 *   AVL=0
 */
#define FLAGS_32BIT   0xC

/* ── Internal: fill one GDT entry ────────────────────────────────────── */
static void gdt_set_entry(int i, u32 base, u32 limit, u8 access, u8 flags)
{
    gdt[i].limit_lo    = (u16)(limit & 0xFFFF);
    gdt[i].base_lo     = (u16)(base  & 0xFFFF);
    gdt[i].base_mid    = (u8)((base  >> 16) & 0xFF);
    gdt[i].access      = access;
    /* Upper nibble = flags, lower nibble = limit bits 19:16 */
    gdt[i].flags_lim_hi = (u8)((flags << 4) | ((limit >> 16) & 0x0F));
    gdt[i].base_hi     = (u8)((base  >> 24) & 0xFF);
}

/* ── Public: initialise and load the GDT ─────────────────────────────── */
void gdt_init(void)
{
    gdt_ptr.limit = (u16)(sizeof(gdt) - 1);
    gdt_ptr.base  = (u32)&gdt;

    gdt_set_entry(0, 0, 0,          0x00,        0x0);      /* null        */
    gdt_set_entry(1, 0, 0xFFFFFFFF, ACCESS_KCODE, FLAGS_32BIT); /* k-code */
    gdt_set_entry(2, 0, 0xFFFFFFFF, ACCESS_KDATA, FLAGS_32BIT); /* k-data */

    gdt_flush((u32)&gdt_ptr);
}
