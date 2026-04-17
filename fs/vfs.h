#pragma once
/* fs/vfs.h — Virtual File System layer (Phase 5) */

#include "../include/types.h"

#define VFS_FILE      0x01
#define VFS_DIRECTORY 0x02

struct vfs_node;
typedef u32 (*read_fn_t) (struct vfs_node *, u32 offset, u32 size, u8 *buf);
typedef u32 (*write_fn_t)(struct vfs_node *, u32 offset, u32 size, u8 *buf);
typedef void (*open_fn_t) (struct vfs_node *);
typedef void (*close_fn_t)(struct vfs_node *);

typedef struct vfs_node {
    char       name[128];
    u32        flags;
    u32        length;
    read_fn_t  read;
    write_fn_t write;
    open_fn_t  open;
    close_fn_t close;
    struct vfs_node *next;  /* sibling in directory */
    struct vfs_node *child; /* first child (if directory) */
} vfs_node_t;

void       vfs_init(vfs_node_t *root);
u32        vfs_read (vfs_node_t *node, u32 offset, u32 size, u8 *buf);
u32        vfs_write(vfs_node_t *node, u32 offset, u32 size, u8 *buf);
vfs_node_t *vfs_open(const char *path);
void        vfs_close(vfs_node_t *node);
