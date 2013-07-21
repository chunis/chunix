#ifndef __FS_H__
#define __FS_H__

#include <types.h>
#include <fs_ext2.h>
#include <fs_initrd.h>

// file mode
#define O_CREAT   1
#define O_RDWR    2

#define NR_SUPER      32
#define BLOCK_SIZE  1024

#define NAME_MAX     255    // # chars in a file name
#define PATH_MAX    1024    // # chars in a path name

#define MAJOR(a) (int)((dev_t)(a) >> 8)
#define MINOR(a) (int)((dev_t)(a) & 0xFF)
#define MKDEV(a,b) ((int)((((a) & 0xff) << 8) | ((b) & 0xff)))
#define NODEV MKDEV(0,0)


enum fs_type {
	INITRD = 1,
	EXT2,
	MINIXFS,
	FAT32,
	SFS,
};

typedef struct super_block *(*read_super_t) (struct super_block *);

struct fs_node {
	read_super_t read_super;
	char *name;
	int req_dev;
	struct fs_node *next;
};

struct super_block {
	dev_t s_dev;
	uint32_t s_flags;
	struct super_operations *s_op;
	struct inode *s_mounted;  // which inode this sb mounted
	struct inode *s_rootnode;  // root node
	union {
		struct ext2_sb_info ext2_sb;
		struct initrd_sb_info initrd_sb;
	} u;
};

struct inode {
	dev_t    i_dev;  // device
	uint32_t i_ino;  // number
	uint16_t i_mode;  // mode: regular file; dir; link; pipe...
	uint16_t i_nlink; // for hard link
	uint8_t  i_dirt;
	uint8_t  i_seek;
	uint8_t  i_update;
	//uid_t    i_uid;
	//gid_t    i_gid;
	//dev_t    i_rdev;
	off_t    i_size;
	//time_t   i_atime;
	//time_t   i_mtime;
	//time_t   i_ctime;
	//uint16_t i_count;
	uint16_t i_flags;
	uint32_t i_blksize;
	uint32_t i_blocks;
	struct inode_operations *i_op;
	struct super_block *i_sb;
	struct inode *i_next, *i_prev;
	//struct inode *i_mount;
	union {
		struct ext2_inode_info ext2_i;
		struct initrd_inode_info initrd_i;
	} u;
};

struct file {
	uint16_t f_mode;
	uint16_t f_flags;
	uint16_t f_count;
	int64_t  f_pos;
	int f_owner;            // pid
	struct file *f_next, *f_prev;
	struct inode *f_inode;
	struct file_operations *f_op;
	void *private_data;     // needed for tty driver, and maybe others
};

struct dirent {
	uint32_t d_ino;
	off_t	 d_off;
	uint16_t d_reclen;
	uint8_t	 d_name[NAME_MAX+1];
};

struct super_operations {
	void (*read_inode) (struct inode *);
	void (*write_inode) (struct inode *);
	void (*put_inode) (struct inode *);
	void (*write_super) (struct super_block *);
	void (*put_super) (struct super_block *);
	int (*remount_fs) (struct super_block *, int *, char *);
};

struct inode_operations {
	struct file_operations *default_file_ops;
	int (*create) (struct inode *, const char *, int, int, struct inode **);
	int (*lookup) (struct inode *, const char *,  int, struct inode **);
	int (*mkdir) (struct inode *, const char *, int, int);
	int (*rmdir) (struct inode *, const char *, int);
	int (*mknod) (struct inode *, const char *, int, int, int);
	int (*rename) (struct inode *, const char *, int, struct inode *, const char *, int);
	void (*truncate) (struct inode *);
	int (*link) (struct inode *, struct inode *, const char *, int);
	int (*unlink) (struct inode *, const char *, int);
	int (*symlink) (struct inode *, const char *, int, const char *);
	int (*readlink) (struct inode *, char *, int);
	int (*follow_link) (struct inode *, struct inode *, int, int, struct inode **);
};

struct file_operations {
	int (*open) (struct inode *, struct file *);
	void (*release) (struct inode *, struct file *);
	int (*read) (struct inode *, struct file *, char *, int);
	int (*write) (struct inode *, struct file *, char *, int);
	int (*lseek) (struct inode *, struct file *, off_t, int);
	int (*readdir) (struct inode *, struct file *, struct dirent *, int);
	int (*ioctl) (struct inode *, struct file *, unsigned int, unsigned long);
};


extern int register_filesystem(struct fs_node *);
extern int unregister_filesystem(struct fs_node *);


#endif
