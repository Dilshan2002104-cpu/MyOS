#pragma once
/* fs/initrd.h — Initial RAM Disk reader (Phase 5) */

#include "../include/types.h"
#include "vfs.h"

vfs_node_t *initrd_init(u32 location);
