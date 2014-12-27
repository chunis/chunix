//
// This file is part of ChuniX and is released under the terms of
// GNU GPLv3 License, see License for details.
//
// Copyright (C) 2014, Chunis Deng (<chunchengfh@gmail.com>)
//
// interface for virtual file system

#ifndef __FS_H__
#define __FS_H__

#include <types.h>

// file mode
#define O_CREAT   1
#define O_RDWR    2

// for device node
#define MAJOR(a) (int)((dev_t)(a) >> 8)
#define MINOR(a) (int)((dev_t)(a) & 0xFF)
#define MKDEV(a,b) ((int)((((a) & 0xff) << 8) | ((b) & 0xff)))
#define NODEV MKDEV(0,0)


#define NAME_MAX     255   // # chars in a file name
#define PATH_MAX     1024  // # chars in a path name
#define BLOCK_SIZE   1024


#define FS_FILE      0x01
#define FS_DIR       0x02
#define FS_CHRDEV    0x03
#define FS_BLKDEV    0x04
#define FS_PIPE      0x05
#define FS_SYMLINK   0x06
#define FS_MNTPOINT  0x10  // mount point for other fs


struct fs_node;
extern struct fs_node *fs_root;	// the root of the file system


// callbacks for fs node
typedef void (*fs_node_open)(struct fs_node *, int);
typedef void (*fs_node_close)(struct fs_node *);
typedef uint32_t (*fs_node_read)(struct fs_node *, uint32_t, uint32_t, char *);
typedef uint32_t (*fs_node_write)(struct fs_node *, uint32_t, uint32_t, char *);
typedef struct dirent *(*fs_node_readdir)(struct fs_node *, uint32_t);
typedef struct fs_node *(*fs_node_finddir)(struct fs_node *, char *);


struct fs_node {
	char name[NAME_MAX];	// file name
	uint32_t mask;		// permissions mask
	uint32_t uid;		// the owning user
	uint32_t gid;		// the owning group
	uint32_t flags;		// node type, and others
	uint32_t inode;
	uint32_t length;	// size of the file
	uint32_t impl;		// implementation-defined number

	fs_node_open open;
	fs_node_close close;
	fs_node_read read;
	fs_node_write write;
	fs_node_readdir readdir;
	fs_node_finddir finddir;
	struct fs_node *ptr;	// for mount points and symbol links
};

struct dirent {
	char name[NAME_MAX];
	uint32_t ino;		// inode number
};


void open_fs(struct fs_node *node, int flag);
void close_fs(struct fs_node *node);
uint32_t read_fs(struct fs_node *node, uint32_t offset, uint32_t size, char *buffer);
uint32_t write_fs(struct fs_node *node, uint32_t offset, uint32_t size, char *buffer);
struct dirent *readdir_fs(struct fs_node *node, uint32_t index);
struct fs_node *finddir_fs(struct fs_node *node, char *name);


#endif
