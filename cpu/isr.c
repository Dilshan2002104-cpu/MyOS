

#include "isr.h"
#include "idt.h"
#include "gdt.h"
#include "../kernel/kprintf.h"
#include "../drivers/vga.h"

extern void isr0(void);  extern void isr1(void);  extern void isr2(void);
extern void isr3(void);  extern void isr4(void);  extern void isr5(void);
extern void isr6(void);  extern void isr7(void);  extern void isr8(void);
extern void isr9(void);  extern void isr10(void); extern void isr11(void);
extern void isr12(void); extern void isr13(void); extern void isr14(void);
extern void isr15(void); extern void isr16(void); extern void isr17(void);
extern void isr18(void); extern void isr19(void); extern void isr20(void);
extern void isr21(void); extern void isr22(void); extern void isr23(void);
extern void isr24(void); extern void isr25(void); extern void isr26(void);
extern void isr27(void); extern void isr28(void); extern void isr29(void);
extern void isr30(void); extern void isr31(void);

static const char *exception_names[32] = {
    [0]  = "Division by Zero",
    [1]  = "Debug",
    [2]  = "Non-Maskable Interrupt",
    [3]  = "Breakpoint",
    [4]  = "Overflow",
    [5]  = "Bound Range Exceeded",
    [6]  = "Invalid Opcode",
    [7]  = "Device Not Available",
    [8]  = "Double Fault",
    [9]  = "Coprocessor Segment Overrun",
    [10] = "Invalid TSS",
    [11] = "Segment Not Present",
    [12] = "Stack-Segment Fault",
    [13] = "General Protection Fault",
    [14] = "Page Fault",
    [15] = "Reserved",
    [16] = "x87 Floating-Point Exception",
    [17] = "Alignment Check",
    [18] = "Machine Check",
    [19] = "SIMD Floating-Point Exception",
    [20] = "Virtualization Exception",
    [21] = "Control Protection Exception",
    [22] = "Reserved", [23] = "Reserved",
    [24] = "Reserved", [25] = "Reserved",
    [26] = "Reserved", [27] = "Reserved",
    [28] = "Hypervisor Injection Exception",
    [29] = "VMM Communication Exception",
    [30] = "Security Exception",
    [31] = "Reserved",
};

static isr_handler_t handlers[32];

void isr_init(void)
{
    
    static void (*stubs[32])(void) = {
        isr0,  isr1,  isr2,  isr3,  isr4,  isr5,  isr6,  isr7,
        isr8,  isr9,  isr10, isr11, isr12, isr13, isr14, isr15,
        isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
        isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31,
    };

    for (int i = 0; i < 32; i++)
        idt_set_gate((u8)i, (u32)stubs[i], GDT_SEL_KCODE, IDT_GATE_INTERRUPT);
}

void isr_register_handler(u8 num, isr_handler_t handler)
{
    if (num < 32)
        handlers[num] = handler;
}

void isr_handler(registers_t *regs)
{
    if (regs->int_no < 32 && handlers[regs->int_no]) {
        
        handlers[regs->int_no](regs);
        return;
    }

    vga_set_color(VGA_WHITE, VGA_RED);
    kputs("\n\n  *** KERNEL EXCEPTION ***\n");

    const char *name = (regs->int_no < 32)
                       ? exception_names[regs->int_no]
                       : "Unknown";

    kprintf("  [#%u] %s\n", regs->int_no, name);

    if (regs->int_no == 14) {
        
        u32 cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        kprintf("  CR2 (faulting address): 0x%x\n", cr2);
    }

    kprintf("  Error code: 0x%x\n",  regs->err_code);
    kprintf("\n  Register dump:\n");
    kprintf("  EAX=0x%x  EBX=0x%x  ECX=0x%x  EDX=0x%x\n",
            regs->eax, regs->ebx, regs->ecx, regs->edx);
    kprintf("  ESI=0x%x  EDI=0x%x  EBP=0x%x  ESP=0x%x\n",
            regs->esi, regs->edi, regs->ebp, regs->esp);
    kprintf("  EIP=0x%x  CS=0x%x  EFLAGS=0x%x  DS=0x%x\n",
            regs->eip, regs->cs, regs->eflags, regs->ds);

    kputs("\n  System halted.");

    __asm__ volatile("cli; hlt");
    while (1) {}
}
