# MyOS

A minimal 32-bit x86 operating system built from scratch in C and NASM Assembly.

## Status

| Phase | Feature | Status |
|-------|---------|--------|
| 0 | Bare-bones Multiboot kernel | ✅ Done |
| 1 | VGA terminal driver + `kprintf` | ⏳ Planned |
| 2 | GDT, IDT, Interrupts (ISR/IRQ) | ⏳ Planned |
| 3 | Physical memory manager + Paging + Heap | ⏳ Planned |
| 4 | PIT timer + PS/2 Keyboard driver | ⏳ Planned |
| 5 | Virtual File System + initrd | ⏳ Planned |
| 6 | User mode (Ring 3) + ELF loader | ⏳ Planned |
| 7 | System calls (`int 0x80`) | ⏳ Planned |
| 8 | Interactive shell | ⏳ Planned |

## Project Structure

```
myos/
├── boot/           # Multiboot assembly entry point
├── kernel/         # Core kernel (main, kprintf, syscalls)
├── cpu/            # GDT, IDT, ISR, IRQ handlers
├── mm/             # Memory management (PMM, paging, heap)
├── drivers/        # VGA, keyboard, PIT
├── fs/             # Virtual file system + initrd
├── include/        # Shared headers (types, I/O helpers)
├── user/           # User-space programs and libc
├── tools/          # Host-side build tools (mkinitrd)
├── linker.ld       # Linker script (loads kernel at 1 MiB)
└── Makefile        # Auto-discovering build system
```

## Building

### Dependencies (Ubuntu/Debian)

```bash
sudo apt-get install -y nasm gcc-multilib binutils qemu-system-x86
```

### Build & Run

```bash
make          # Compile and link → kernel.elf
make run      # Build + launch in QEMU
make clean    # Remove all build artefacts
```

## How It Works

1. **GRUB/QEMU** reads the Multiboot header embedded in `boot/boot.s`
2. The bootloader jumps to `_start`, which sets up a 16 KiB stack
3. Control passes to `kernel_main()` in `kernel/kernel.c`
4. The kernel writes directly to the VGA text buffer at `0xB8000`

## Tech Stack

- **Language:** C11 (freestanding) + NASM x86 Assembly
- **Target:** 32-bit x86 (IA-32), Multiboot-compliant
- **Toolchain:** `gcc -m32 -ffreestanding`, `nasm -f elf32`, `ld`
- **Emulator:** QEMU (`qemu-system-x86_64 -kernel kernel.elf`)

## License

MIT
