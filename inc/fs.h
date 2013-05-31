#ifndef __FS_H__
#define __FS_H__

typedef struct super_block *(*read_super_t) (struct super_block *, void *, int);

struct fs_node {
	read_super_t read_super;
	char *name;
	int requires_dev;
	struct fs_node * next;
};

extern int register_filesystem(struct fs_node *);

#endif
