; cpu/isr.s — CPU exception stub trampolines + common handler
;
; Two macro flavours:
;   ISR_NOERRCODE n — CPU does NOT push an error code for this exception.
;                     We push a dummy 0 so the stack frame is uniform.
;   ISR_ERRCODE   n — CPU pushes an error code automatically.
;                     We only push the interrupt number.
;
; Exceptions with CPU-pushed error codes: 8, 10, 11, 12, 13, 14, 17
; All others: no error code.
;
; Phase 2.

bits 32
section .note.GNU-stack noalloc noexec nowrite progbits
section .text

; ── Macros ────────────────────────────────────────────────────────────────────

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0        ; dummy error code
    push dword %1       ; interrupt number
    jmp  isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push dword %1       ; interrupt number (CPU already pushed error code)
    jmp  isr_common_stub
%endmacro

; ── CPU Exception Stubs (vectors 0-31) ────────────────────────────────────────
ISR_NOERRCODE  0   ; Divide-by-Zero
ISR_NOERRCODE  1   ; Debug
ISR_NOERRCODE  2   ; Non-Maskable Interrupt
ISR_NOERRCODE  3   ; Breakpoint
ISR_NOERRCODE  4   ; Overflow
ISR_NOERRCODE  5   ; Bound Range Exceeded
ISR_NOERRCODE  6   ; Invalid Opcode
ISR_NOERRCODE  7   ; Device Not Available (FPU)
ISR_ERRCODE    8   ; Double Fault          ← has error code
ISR_NOERRCODE  9   ; Coprocessor Segment Overrun (legacy)
ISR_ERRCODE   10   ; Invalid TSS           ← has error code
ISR_ERRCODE   11   ; Segment Not Present   ← has error code
ISR_ERRCODE   12   ; Stack-Segment Fault   ← has error code
ISR_ERRCODE   13   ; General Protection Fault ← has error code
ISR_ERRCODE   14   ; Page Fault            ← has error code
ISR_NOERRCODE 15   ; Reserved
ISR_NOERRCODE 16   ; x87 Floating-Point Exception
ISR_ERRCODE   17   ; Alignment Check       ← has error code
ISR_NOERRCODE 18   ; Machine Check
ISR_NOERRCODE 19   ; SIMD Floating-Point Exception
ISR_NOERRCODE 20   ; Virtualization Exception
ISR_NOERRCODE 21   ; Control Protection Exception
ISR_NOERRCODE 22   ; Reserved
ISR_NOERRCODE 23   ; Reserved
ISR_NOERRCODE 24   ; Reserved
ISR_NOERRCODE 25   ; Reserved
ISR_NOERRCODE 26   ; Reserved
ISR_NOERRCODE 27   ; Reserved
ISR_NOERRCODE 28   ; Hypervisor Injection Exception
ISR_NOERRCODE 29   ; VMM Communication Exception
ISR_NOERRCODE 30   ; Security Exception
ISR_NOERRCODE 31   ; Reserved

; ── Common stub ──────────────────────────────────────────────────────────────
; At entry, the stack holds (from ESP upward):
;   [esp+0]  int_no
;   [esp+4]  err_code (real or dummy 0)
;   [esp+8]  eip   ┐ pushed by CPU
;   [esp+12] cs    │
;   [esp+16] eflags┘

extern isr_handler   ; defined in isr.c

isr_common_stub:
    pusha               ; push edi,esi,ebp,orig_esp,ebx,edx,ecx,eax

    mov ax, ds          ; save current data segment
    push eax            ; pushed as 32-bit (upper 16 = 0)

    ; Load the kernel data segment so our C handler runs in ring-0 context
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp            ; pass registers_t * as argument to isr_handler
    call isr_handler
    add  esp, 4         ; discard argument

    pop  eax            ; restore saved data segment
    mov  ds, ax
    mov  es, ax
    mov  fs, ax
    mov  gs, ax

    popa                ; restore edi,esi,ebp,esp,ebx,edx,ecx,eax
    add  esp, 8         ; discard int_no and err_code
    iret                ; restore eip, cs, eflags (+ useresp,ss on ring change)
