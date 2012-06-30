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

struct superblock {
	uint8_t  rev_boot1[11];
	uint8_t  rev_bios[21];
	uint8_t  rev_boot2[372];
	uint64_t time_stamp;
	uint64_t da_blk;
	uint64_t ia_size;
	uint8_t  magic_num[3];
	uint8_t  fs_version;
	uint64_t total_blk;
	uint32_t rev_blk;
	uint8_t  blk_size;
	uint8_t  checksum;
	uint8_t  rev_pt[64];  // partition table
	uint8_t  rev_boot_sig[2];  // boot signature
}__attribute__((packed));

#endif
