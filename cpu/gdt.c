

#include "gdt.h"

#define GDT_ENTRIES 3

static gdt_entry_t gdt[GDT_ENTRIES];
static gdt_ptr_t   gdt_ptr;

extern void gdt_flush(u32 gdt_ptr_addr);

#define ACCESS_KCODE  0x9A   
#define ACCESS_KDATA  0x92   

#define FLAGS_32BIT   0xC

static void gdt_set_entry(int i, u32 base, u32 limit, u8 access, u8 flags)
{
    gdt[i].limit_lo    = (u16)(limit & 0xFFFF);
    gdt[i].base_lo     = (u16)(base  & 0xFFFF);
    gdt[i].base_mid    = (u8)((base  >> 16) & 0xFF);
    gdt[i].access      = access;
    
    gdt[i].flags_lim_hi = (u8)((flags << 4) | ((limit >> 16) & 0x0F));
    gdt[i].base_hi     = (u8)((base  >> 24) & 0xFF);
}

void gdt_init(void)
{
    gdt_ptr.limit = (u16)(sizeof(gdt) - 1);
    gdt_ptr.base  = (u32)&gdt;

    gdt_set_entry(0, 0, 0,          0x00,        0x0);      
    gdt_set_entry(1, 0, 0xFFFFFFFF, ACCESS_KCODE, FLAGS_32BIT); 
    gdt_set_entry(2, 0, 0xFFFFFFFF, ACCESS_KDATA, FLAGS_32BIT); 

    gdt_flush((u32)&gdt_ptr);
}
