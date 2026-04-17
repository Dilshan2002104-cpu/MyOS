; cpu/irq.s — Hardware IRQ stub trampolines + common handler
;
; IRQ 0-15 are remapped by the PIC to IDT vectors 32-47.
; Each stub pushes a dummy error code (IRQs have none) and the
; vector number, then jumps to the shared irq_common_stub.
;
; Phase 2.

bits 32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text

; ── Macro: one stub per IRQ ───────────────────────────────────────────────────

; %1 = IRQ number (0-15), %2 = IDT vector (32-47)
%macro IRQ 2
global irq%1
irq%1:
    push dword 0        ; dummy error code (IRQs don't produce one)
    push dword %2       ; vector number in IDT
    jmp  irq_common_stub
%endmacro

; ── IRQ Stubs ──────────────────────────────────────────────────────────────
IRQ  0, 32   ; PIT Timer
IRQ  1, 33   ; PS/2 Keyboard
IRQ  2, 34   ; Cascade (slave PIC) — not directly usable
IRQ  3, 35   ; COM2
IRQ  4, 36   ; COM1
IRQ  5, 37   ; LPT2 / Sound card
IRQ  6, 38   ; Floppy disk
IRQ  7, 39   ; LPT1 / Spurious
IRQ  8, 40   ; CMOS Real-Time Clock
IRQ  9, 41   ; Free / ACPI
IRQ 10, 42   ; Free
IRQ 11, 43   ; Free
IRQ 12, 44   ; PS/2 Mouse
IRQ 13, 45   ; FPU / Coprocessor
IRQ 14, 46   ; Primary ATA
IRQ 15, 47   ; Secondary ATA

; ── Common stub ───────────────────────────────────────────────────────────────
extern irq_handler   ; defined in irq.c

irq_common_stub:
    pusha               ; save edi,esi,ebp,orig_esp,ebx,edx,ecx,eax

    mov ax, ds
    push eax            ; save data segment

    mov ax, 0x10        ; kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp            ; pass registers_t * to irq_handler
    call irq_handler
    add  esp, 4

    pop  eax
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    popa
    add  esp, 8         ; discard dummy error code and vector number
    iret
