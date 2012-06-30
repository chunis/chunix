#ifndef __FS_H__
#define __FS_H__

#include <types.h>

int open(const char *pathname, int flags);
int creat(const char *pathname, int flags);
int read(int fd, void *buf, int n);
int write(int fd, const void *buf, int n);
int close(int fd);

struct buf {
	int flag;
	uint32_t dev;
	uint32_t num;
	uint32_t data[512];

	struct buf *prev;
	struct buf *next;
	struct buf *fnext;
};

// buffer flag
#define BUF_BUSY  0x1	// buffer is locked
#define BUF_VALID 0x2	// buffer data is valid
#define BUF_DIRTY 0x4	// buffer data needs to be written to disk

// file mode
#define O_CREAT   1
#define O_RDWR    2

#define MINIX_MAGIC	0x137F    // minix v1 fs, 14 char names
#define MINIX_MAGIC2	0x138F    // minix v1 fs, 30 char names

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

#endif
