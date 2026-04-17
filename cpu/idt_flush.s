; cpu/idt_flush.s — Load the IDTR
;
; Called from C as: void idt_flush(u32 idt_ptr_addr);
;
; Phase 2.

bits 32
section .note.GNU-stack noalloc noexec nowrite progbits

section .text
global idt_flush

idt_flush:
    mov eax, [esp+4]    ; get pointer to idt_ptr_t struct
    lidt [eax]          ; load the IDT
    ret
