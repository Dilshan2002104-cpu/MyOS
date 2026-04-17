; cpu/gdt_flush.s — Load the GDT and reload segment registers
;
; Called from C as: void gdt_flush(u32 gdt_ptr_addr);
; The single argument (GDTR pointer) is passed on the stack at [esp+4].
;
; After lgdt, we must do a far jump to reload CS with the new code selector.
; All other segment registers are loaded with the kernel data selector (0x10).
;
; Phase 2.

bits 32
section .note.GNU-stack noalloc noexec nowrite progbits

section .text
global gdt_flush

gdt_flush:
    mov eax, [esp+4]    ; get pointer to gdt_ptr_t struct
    lgdt [eax]          ; load the GDT

    ; Reload all data segment registers with kernel data selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS with kernel code selector (0x08).
    ; This is the only way to change CS in protected mode.
    jmp 0x08:.reload_cs

.reload_cs:
    ret
