; cpu/gdt_flush.s — Load the GDT and reload segment registers (Phase 2)
; TODO: Implement in Phase 2
bits 32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text
global gdt_flush
gdt_flush:
    ret  ; placeholder
