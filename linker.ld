/* linker.ld — Linker script for the bare-bones x86 kernel
 *
 * Key constraints:
 *  • Entry point must be _start (defined in boot.s)
 *  • Load address 1 MiB (0x100000) — conventional for Multiboot kernels;
 *    keeps us above the legacy BIOS/ISA regions in the first megabyte.
 *  • .multiboot MUST be the very first thing so GRUB/QEMU can find the
 *    Multiboot magic within the first 8 KiB of the image.
 */

ENTRY(_start)

SECTIONS
{
    /*
     * Start at 1 MiB.  The dot (.) is the location counter — every section
     * placed after this point will be laid out contiguously from here.
     */
    . = 1M;

    /* ── Read-Only Code ───────────────────────────────────────────────────── */
    .text BLOCK(4K) : ALIGN(4K)
    {
        /*
         * The Multiboot header must be 4-byte aligned and within the first
         * 8 KiB of the kernel image.  Place it first inside .text.
         */
        *(.multiboot)
        *(.text)
    }

    /* ── Read-Only Data (string literals, const globals, etc.) ─────────────── */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* ── Initialised Read-Write Data ────────────────────────────────────────── */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /*
     * ── Zero-Initialised (BSS) ────────────────────────────────────────────────
     *
     * The BSS section is NOT stored in the binary — Multiboot loaders zeroise
     * it at load time.  The COMMON symbol holds tentative (uninitialized)
     * global variables in C.
     */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }

    /*
     * Discard everything the toolchain emits that we don't need
     * (DWARF debug info, exception tables, etc.).
     */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
