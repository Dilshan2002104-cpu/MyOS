

AS      := nasm
CC      := gcc
LD      := ld

ASFLAGS := -f elf32
CFLAGS  := -m32 -ffreestanding -fno-stack-protector -fno-pic \
           -Wall -Wextra -Wno-unused-parameter -std=c11 -O2 \
           -I./include -I./kernel -I./cpu -I./mm -I./drivers -I./fs
LDFLAGS := -m elf_i386 -T linker.ld

TARGET  := kernel.elf

C_SRCS   := $(shell find kernel cpu mm drivers fs -name '*.c' 2>/dev/null)

ASM_SRCS := $(sort boot/boot.s $(shell find cpu -name '*.s' 2>/dev/null))

C_OBJS   := $(patsubst %.c, build/%.o, $(C_SRCS))
ASM_OBJS := $(patsubst %.s, build/%.o, $(ASM_SRCS))
OBJS     := $(sort $(ASM_OBJS) $(C_OBJS))

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@
	@echo ""
	@echo "  ╔═══════════════════════════════╗"
	@echo "  ║   Build successful: $@         ║"
	@echo "  ╚═══════════════════════════════╝"
	@echo ""

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

INITRD  := initrd.img

run: all $(INITRD)
	qemu-system-x86_64 -kernel $(TARGET) -initrd $(INITRD)

$(INITRD): tools/make_initrd.c
	@gcc tools/make_initrd.c -o tools/make_initrd
	@echo "Hello from Initrd!" > test.txt
	@./tools/make_initrd $(INITRD) test.txt test.txt

clean:
	rm -rf build/ $(TARGET) $(INITRD) tools/make_initrd test.txt
