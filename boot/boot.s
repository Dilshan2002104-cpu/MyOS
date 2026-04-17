

bits 32

MULTIBOOT_MAGIC     equ 0x1BADB002
MULTIBOOT_FLAGS     equ 0x00000003   
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .note.GNU-stack noalloc noexec nowrite progbits

section .bss
align 16
stack_bottom:
    resb 16384          
stack_top:

section .text
global _start           
extern kernel_main      

_start:
    
    mov esp, stack_top

    push 0
    popf

    push ebx
    push eax

    call kernel_main

.halt:
    cli                 
    hlt                 
    jmp .halt           
