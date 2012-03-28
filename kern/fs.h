/*
  fs.c: A trival implement for SFS (Simple File System).
  Refer to: http://dimensionalrift.homelinux.net/combuster/vdisk/sfs.html
*/

#ifndef __FS_H__
#define __FS_H__

#include <types.h>

int open(const char *pathname, int flags);
int read(int fd, void *buf, int n);
int write(int fd, const void *buf, int n);
int close(int fd);

// Number of Index entries needed for a name str whose length is 'len'
#define NIE(len) (((len)+34)/IE_SIZE + 1)

#define OFILE  10	// open file for each process
#define FDT_SIZE 64	// size of fdtable[]

#define IE_SIZE 64	// Index entries size is 64 bytes each
#define NB_INIT 4	// blocks for file when created
#define NB_MORE 8	// blocks for file when it overflow its init space

// buffer size for inode cache array
#define IBUF1  100
#define IBUF2  30
#define IBUF3  8
#define IBUF4  4

#define O_CREAT   1
#define O_RDWR    2

// Layout of areas
#define SUPER_BLOCK   0x1
#define RESERVED_AREA   0x2
#define DATA_AREA    0x3
#define FREE_AREA    0x4
#define INDEX_AREA   0x5

// Index area entries type
#define VOLUME_ID     0x01
#define START_MARK    0x02
#define UNUSED_ENTRY  0x10
#define DIR_ENTRY     0x11
#define FILE_ENTRY    0x12
#define UNUSAB_ENTRY  0x18
#define DEL_DIR       0x19
#define DEL_FILE      0x1A
#define CONT_ENTRY    0x20	// 0x20 ~ 0xFF

#define T_FILE   FILE_ENTRY
#define T_DIR    DIR_ENTRY

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

struct file_desp {
	int fd_mode;
	int fd_off;
	struct inode1 *fd_inode;
};

struct sfs_vol_id {
	uint8_t etype;
	uint8_t resv[3];
	uint64_t time;
	uint8_t name[52];
}__attribute__((packed));

struct sfs_mark {
	uint8_t etype;
	uint8_t resv[63];
};

struct sfs_unused {
	uint8_t etype;
	uint8_t resv[63];
};

struct sfs_dir {
	uint8_t etype;
	uint8_t ne;  // number of entry
	uint64_t time;
	uint8_t name[54];
}__attribute__((packed));

struct sfs_file {
	uint8_t etype;
	uint8_t ne;  // number of entry
	uint64_t time;
	uint64_t blk_start;
	uint64_t blk_end;
	uint64_t len;
	uint8_t name[30];
}__attribute__((packed));

struct sfs_unusable {
	uint8_t etype;
	uint8_t resv1[9];
	uint64_t blk_start;
	uint64_t blk_end;
	uint8_t resv2[38];
}__attribute__((packed));

// inode cache in memory for files whose index entry size == 64 bytes
struct inode1 {
	int nb;		// block number in hd
	uint8_t ni;		// index number in a single block
	int8_t nref;	// reference count
	int8_t rw;		// read/write flag
	int8_t dirty;	// is this inode changed?

	struct sfs_file sfile;	// index block (64bytes)
}__attribute__((packed));

// inode cache in memory for files whose index entry size == 64*2 bytes
struct inode2 {
	int nb;		// block number in hd
	uint8_t ni;		// index number in a single block
	int8_t nref;	// reference count
	int8_t rw;		// read/write flag
	int8_t dirty;	// is this inode changed?

	struct sfs_file sfile;	// index block (64bytes)
	uint8_t cont_ent[IE_SIZE];
}__attribute__((packed));

// inode cache in memory for files whose index entry size == 64*3 bytes
struct inode3 {
	int nb;		// block number in hd
	uint8_t ni;		// index number in a single block
	int8_t nref;	// reference count
	int8_t rw;		// read/write flag
	int8_t dirty;	// is this inode changed?

	struct sfs_file sfile;	// index block (64bytes)
	uint8_t cont_ent[IE_SIZE*2];
};

// inode cache in memory for files whose index entry size == 64*4 bytes
struct inode4 {
	int nb;		// block number in hd
	uint8_t ni;		// index number in a single block
	int8_t nref;	// reference count
	int8_t rw;		// read/write flag
	int8_t dirty;	// is this inode changed?

	struct sfs_file sfile;	// index block (64bytes)
	uint8_t cont_ent[IE_SIZE*3];
}__attribute__((packed));

// use to step index by 64 bytes scale each
struct sfs_index {
	uint8_t etype;	// entry type
	uint8_t data[63];
};

struct file_desp fdtable[FDT_SIZE];

struct inode1 ibuf1[IBUF1];
struct inode2 ibuf2[IBUF2];
struct inode3 ibuf3[IBUF3];
struct inode4 ibuf4[IBUF4];

#endif
