# Makefile — MyOS build system
# Automatically discovers all .c and .s sources in the project tree.
# Usage:
#   make          — build kernel.elf
#   make run      — build + launch QEMU
#   make clean    — remove all build artefacts

# ── Toolchain ─────────────────────────────────────────────────────────────────
AS      := nasm
CC      := gcc
LD      := ld

ASFLAGS := -f elf32
CFLAGS  := -m32 -ffreestanding -fno-stack-protector -fno-pic \
           -Wall -Wextra -std=c11 -O2 \
           -I./include -I./kernel -I./cpu -I./mm -I./drivers -I./fs
LDFLAGS := -m elf_i386 -T linker.ld

TARGET  := kernel.elf

# ── Source Discovery ──────────────────────────────────────────────────────────
# Collect every .c in the kernel-space directories (exclude tools/ — host code)
C_SRCS   := $(shell find kernel cpu mm drivers fs -name '*.c' 2>/dev/null)
# Boot entry + CPU ASM stubs (deduplicate with sort)
ASM_SRCS := $(sort boot/boot.s $(shell find cpu -name '*.s' 2>/dev/null))

# ── Object Files (mirrors source tree under build/) ──────────────────────────
C_OBJS   := $(patsubst %.c, build/%.o, $(C_SRCS))
ASM_OBJS := $(patsubst %.s, build/%.o, $(ASM_SRCS))
OBJS     := $(sort $(ASM_OBJS) $(C_OBJS))

# ── Default Target ────────────────────────────────────────────────────────────
.PHONY: all run clean

all: $(TARGET)

# ── Link ──────────────────────────────────────────────────────────────────────
$(TARGET): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@
	@echo ""
	@echo "  ╔═══════════════════════════════╗"
	@echo "  ║   Build successful: $@         ║"
	@echo "  ╚═══════════════════════════════╝"
	@echo ""

# ── Compile .c → build/xxx.o ──────────────────────────────────────────────────
build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ── Assemble .s → build/xxx.o ─────────────────────────────────────────────────
build/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# ── Run ───────────────────────────────────────────────────────────────────────
run: all
	qemu-system-x86_64 -kernel $(TARGET)

# ── Clean ─────────────────────────────────────────────────────────────────────
clean:
	rm -rf build/ $(TARGET)
