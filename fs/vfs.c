

#include "vfs.h"

fs_node_t *fs_root = 0;

u32 vfs_read(fs_node_t *node, u32 offset, u32 size, u8 *buffer) {
    if (node->read != 0)
        return node->read(node, offset, size, buffer);
    else
        return 0;
}

u32 vfs_write(fs_node_t *node, u32 offset, u32 size, u8 *buffer) {
    if (node->write != 0)
        return node->write(node, offset, size, buffer);
    else
        return 0;
}

void vfs_open(fs_node_t *node, u8 read, u8 write) {
    if (node->open != 0)
        return node->open(node);
}

void vfs_close(fs_node_t *node) {
    if (node->close != 0)
        return node->close(node);
}

struct dirent *vfs_readdir(fs_node_t *node, u32 index) {
    if ((node->flags & 0x07) == FS_DIRECTORY && node->readdir != 0)
        return node->readdir(node, index);
    else
        return 0;
}

fs_node_t *vfs_finddir(fs_node_t *node, char *name) {
    if ((node->flags & 0x07) == FS_DIRECTORY && node->finddir != 0)
        return node->finddir(node, name);
    else
        return 0;
}
