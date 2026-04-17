#pragma once
/* kernel/syscall.h — System call dispatcher (Phase 7) */

#include "../include/types.h"

/* Syscall numbers */
#define SYS_EXIT   0
#define SYS_WRITE  1
#define SYS_READ   2
#define SYS_OPEN   3
#define SYS_CLOSE  4

void syscall_init(void);
void syscall_handler(void *regs);
