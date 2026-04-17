#pragma once

#include "../include/types.h"

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02

struct fs_node;

typedef u32 (*read_type_t)(struct fs_node*, u32, u32, u8*);
typedef u32 (*write_type_t)(struct fs_node*, u32, u32, u8*);
typedef void (*open_type_t)(struct fs_node*);
typedef void (*close_type_t)(struct fs_node*);
typedef struct dirent * (*readdir_type_t)(struct fs_node*, u32);
typedef struct fs_node * (*finddir_type_t)(struct fs_node*, char *name);

typedef struct fs_node {
    char name[128];     
    u32 mask;           
    u32 uid;            
    u32 gid;            
    u32 flags;          
    u32 inode;          
    u32 length;         
    u32 impl;           
    
    read_type_t read;
    write_type_t write;
    open_type_t open;
    close_type_t close;
    readdir_type_t readdir;
    finddir_type_t finddir;
    
    struct fs_node *ptr; 
} fs_node_t;

struct dirent {
    char name[128];
    u32 ino;            
};

extern fs_node_t *fs_root; 

u32 vfs_read(fs_node_t *node, u32 offset, u32 size, u8 *buffer);
u32 vfs_write(fs_node_t *node, u32 offset, u32 size, u8 *buffer);
void vfs_open(fs_node_t *node, u8 read, u8 write);
void vfs_close(fs_node_t *node);
struct dirent *vfs_readdir(fs_node_t *node, u32 index);
fs_node_t *vfs_finddir(fs_node_t *node, char *name);
