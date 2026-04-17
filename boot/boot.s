; boot.s — Multiboot-compliant entry point for the kernel
; Assembled with NASM

bits 32

; ─── Multiboot Header Constants ───────────────────────────────────────────────
MULTIBOOT_MAGIC     equ 0x1BADB002
MULTIBOOT_FLAGS     equ 0x00000003   ; flag bits 0+1: align modules, give memory map
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; ─── Multiboot Header Section ─────────────────────────────────────────────────
section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

; ─── Mark stack as non-executable (silences GNU ld warning) ───────────────────
section .note.GNU-stack noalloc noexec nowrite progbits


; ─── BSS: Uninitialized Stack Storage ─────────────────────────────────────────
section .bss
align 16
stack_bottom:
    resb 16384          ; 16 KiB stack
stack_top:

; ─── Text: Kernel Entry Point ─────────────────────────────────────────────────
section .text
global _start           ; expose to linker
extern kernel_main      ; defined in kernel.c

_start:
    ; Set up the stack pointer — stacks grow downward on x86
    mov esp, stack_top

    ; Clear EFLAGS (direction flag, interrupt flag, etc.)
    push 0
    popf

    ; Push Multiboot info pointer and magic number as arguments to kernel_main
    push ebx
    push eax

    ; Call the C kernel entry point
    call kernel_main

    ; If kernel_main returns (it shouldn't), halt the CPU permanently
.halt:
    cli                 ; disable interrupts
    hlt                 ; halt the CPU
    jmp .halt           ; guard against spurious NMIs waking us up
