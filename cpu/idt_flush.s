

bits 32
section .note.GNU-stack noalloc noexec nowrite progbits

section .text
global idt_flush

idt_flush:
    mov eax, [esp+4]    
    lidt [eax]          
    ret
