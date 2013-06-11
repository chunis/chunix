#ifndef __BUF_H__
#define __BUF_H__

struct buf {
	dev_t dev;
	int flags;
	uint32_t num;

	void *bwait;
	struct buf *prev;
	struct buf *next;
	struct buf *hdnext;

	uint8_t data[512];
};

// buffer flag
#define BUF_BUSY  0x1	// buffer is locked
#define BUF_VALID 0x2	// buffer data is valid
#define BUF_DIRTY 0x4	// buffer data needs to be written to disk

// Device Major number (keep it the same as Linux)
#define UNNAMED_MAJOR   0
#define MEM_MAJOR       1
#define IDE0_MAJOR      3
#define HD_MAJOR        IDE0_MAJOR
#define TTY_MAJOR       4
#define SCSI_DISK_MAJOR 8
#define SCSI_TAPE_MAJOR 9
#define MOUSE_MAJOR     10
#define SCSI_CDROM_MAJOR 11
#define SOUND_MAJOR     14
#define SOCKET_MAJOR    16
#define AF_UNIX_MAJOR   17
#define AF_INET_MAJOR   18
#define SCSI_GENERIC_MAJOR 21
#define IDE1_MAJOR      22


#endif
