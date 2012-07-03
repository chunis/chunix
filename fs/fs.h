#ifndef __FS_H__
#define __FS_H__

#include <types.h>

// buffer flag
#define BUF_BUSY  0x1	// buffer is locked
#define BUF_VALID 0x2	// buffer data is valid
#define BUF_DIRTY 0x4	// buffer data needs to be written to disk

// file mode
#define O_CREAT   1
#define O_RDWR    2

#define MINIX_MAGIC	0x137F    // minix v1 fs, 14 char names
#define MINIX_MAGIC2	0x138F    // minix v1 fs, 30 char names

// minixfs with 30-chars dirs
#define DIR_LEN		30
#define NDIRECT		7	// number of direct blocks
#define NINDIRECT	1	// number of indirect blocks
#define NDINDIRECT	1	// number of double indirect blocks
#define ADDR_LEN	(NDIRECT + NINDIRECT + NDINDIRECT)

#define ROOTDEV		1	// dev number of fs root disk
#define ROOTINO		1	// root inode number

struct buf {
	int flag;
	uint32_t dev;
	uint32_t num;
	uint32_t data[512];

	struct buf *prev;
	struct buf *next;
	struct buf *fnext;
};

// superblock saved in disk
struct d_superblock {
	uint16_t s_ninodes;
	uint16_t s_nzones;
	uint16_t s_imap_blks;
	uint16_t s_zmap_blks;
	uint16_t s_firstdatazone;
	uint16_t s_log_zone_size;
	uint32_t s_max_size;
	uint16_t s_magic;
};

// superblock maintained in memory
struct superblock {
	uint16_t s_ninodes;
	uint16_t s_nzones;
	uint16_t s_imap_blks;
	uint16_t s_zmap_blks;
	uint16_t s_firstdatazone;
	uint16_t s_log_zone_size;
	uint32_t s_max_size;
	uint16_t s_magic;

	// below members only in memory
	uint16_t s_dev;
	uint16_t s_flag;
	uint32_t s_time;
	//struct buf *s_imap[8];
	//struct buf *s_zmap[8];
	//struct m_inode *s_isup;
	//struct m_inode *s_imount;
	//struct task_struct *s_wait;
}__attribute__((packed));

// inode data saved in disk
struct d_inode {
	uint16_t type;
	uint16_t uid;
	uint32_t size;
	uint32_t time;
	uint8_t  gid;
	uint8_t  count;
	uint16_t addr[ADDR_LEN];
};

// inode data maintained in memory
struct inode {
	uint16_t type;
	uint16_t uid;
	uint32_t size;
	uint32_t time;
	uint8_t  gid;
	uint8_t  count;
	uint16_t addr[ADDR_LEN];

	// below members only in memory
	uint16_t dev;
	uint16_t inum;
	uint16_t ref;
};

struct file {
	uint32_t mode;
	uint32_t flags;
	uint32_t ref;	// reference count
	uint32_t pos;
	struct inode *inode;
};

struct dirent {
	uint16_t inum;
	int8_t name[DIR_LEN];
};

int fileopen(const char *pathname, int flags);
int filecreat(const char *pathname, int flags);
int fileread(int fd, void *buf, int n);
int filewrite(int fd, const void *buf, int n);
int fileclose(int fd);

#endif
