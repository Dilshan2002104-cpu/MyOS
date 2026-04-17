

#include "initrd.h"
#include "../mm/heap.h"
#include "../include/string.h"
#include "../kernel/kprintf.h"

typedef struct {
    u32 nfiles;
} initrd_header_t;

typedef struct {
    u32 magic;
    char name[64];
    u32 offset;
    u32 length;
} initrd_file_header_t;

static initrd_header_t *initrd_header;
static initrd_file_header_t *file_headers;
static fs_node_t *initrd_root;
static fs_node_t *root_nodes;
static u32 nroot_nodes;

static struct dirent dirent;

static u32 initrd_read(fs_node_t *node, u32 offset, u32 size, u8 *buffer) {
    initrd_file_header_t header = file_headers[node->inode];
    if (offset > header.length) return 0;
    if (offset + size > header.length) size = header.length - offset;
    
    memcpy(buffer, (u8*)(header.offset + offset), size);
    return size;
}

static struct dirent *initrd_readdir(fs_node_t *node, u32 index) {
    if (node == initrd_root && index < nroot_nodes) {
        strcpy(dirent.name, root_nodes[index].name);
        dirent.ino = root_nodes[index].inode;
        return &dirent;
    }
    return 0;
}

static fs_node_t *initrd_finddir(fs_node_t *node, char *name) {
    if (node == initrd_root) {
        for (u32 i = 0; i < nroot_nodes; i++) {
            if (!strcmp(name, root_nodes[i].name))
                return &root_nodes[i];
        }
    }
    return 0;
}

fs_node_t *initrd_init(u32 location) {
    initrd_header = (initrd_header_t *)location;
    file_headers = (initrd_file_header_t *)(location + sizeof(initrd_header_t));

    initrd_root = (fs_node_t *)kmalloc(sizeof(fs_node_t));
    strcpy(initrd_root->name, "initrd");
    initrd_root->mask = initrd_root->uid = initrd_root->gid = initrd_root->inode = initrd_root->length = 0;
    initrd_root->flags = FS_DIRECTORY;
    initrd_root->read = 0;
    initrd_root->write = 0;
    initrd_root->open = 0;
    initrd_root->close = 0;
    initrd_root->readdir = initrd_readdir;
    initrd_root->finddir = initrd_finddir;
    initrd_root->ptr = 0;
    initrd_root->impl = 0;

    nroot_nodes = initrd_header->nfiles;
    root_nodes = (fs_node_t *)kmalloc(sizeof(fs_node_t) * nroot_nodes);

    for (u32 i = 0; i < nroot_nodes; i++) {
        
        file_headers[i].offset += location;
        
        strcpy(root_nodes[i].name, file_headers[i].name);
        root_nodes[i].mask = root_nodes[i].uid = root_nodes[i].gid = 0;
        root_nodes[i].length = file_headers[i].length;
        root_nodes[i].inode = i;
        root_nodes[i].flags = FS_FILE;
        root_nodes[i].read = initrd_read;
        root_nodes[i].write = 0;
        root_nodes[i].readdir = 0;
        root_nodes[i].finddir = 0;
        root_nodes[i].open = root_nodes[i].close = 0;
        root_nodes[i].impl = 0;
    }
    
    return initrd_root;
}
