
#define DIR_SPACE 54
#define FILE_SPACE 30

// Number of Index entries needed for a name str whose length is 'len'
#define NIE_FILE(len) (((len)+34+1)/IE_SIZE) // first 1 byte for '\0'
#define NIE_DIR(len) (((len)+10+1)/IE_SIZE) // first 1 byte for '\0'

#define IE_SIZE 64	// Index entries size is 64 bytes each
#define NB_INIT 4	// blocks for file when created
#define NB_MORE 8	// blocks for file when it overflow its init space

//typedef signed char int8_t;
typedef unsigned char uint8_t;
//typedef short int16_t;
//typedef unsigned short uint16_t;
//typedef int int32_t;
typedef unsigned int uint32_t;
//typedef long long int64_t;
typedef unsigned long long uint64_t;

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

struct sfs_superblock {
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

/*
struct file_desp {
	int fd_mode;
	int fd_off;
	struct sfs_inode *fd_inode;
};
*/

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

struct sfs_index {
	uint8_t etype;	// entry type
	uint8_t data[63];
};

// inode cache in memory for index
struct sfs_inode {
	int nb;		// block number in hd
	uint8_t ni;	// index number in a single block
	int8_t nref;	// reference count
	int8_t rw;	// read/write flag
	int8_t dirty;	// is this inode changed?

	struct sfs_index *sindex;	// index block (64bytes)
}__attribute__((packed));
