#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "mk_sfs.h"

#define BLKSZ	512
#define NBLK	(170 * 16 * 63)  // hd size ~= 80M with sb.blk_size=2
#define BLOCK_INDEX(da_num) ((da_num) + reserved_blk)

int sfs_fd;
struct sfs_superblock sb;
uint32_t nblk = NBLK;
uint32_t nindex = NBLK * (BLKSZ / IE_SIZE);
uint32_t blksz = BLKSZ;
uint32_t reserved_blk = 1;
uint32_t da_blocks = 0;  // data area size in blocks
uint32_t ia_num = 0;  // index area size = ia_num * IE_SIZE
struct sfs_mark mark = { START_MARK, };

static char calc_checksum(char *sb)
{
	char sum = 0;
	int cnt;

	for(cnt=0x1ac; cnt<=0x1bd; cnt++)
		sum += sb[cnt];

	return sum;
}

static void read_elem(uint32_t n, uint32_t elem_size, void *buf)
{
	if(lseek(sfs_fd, n * elem_size, 0) != n * elem_size){
		perror("lseek error");
		exit(1);
	}
	if(read(sfs_fd, buf, elem_size) != elem_size){
		perror("read error");
		exit(1);
	}
}

static void write_elem(uint32_t n, uint32_t elem_size, void *buf)
{
	if(lseek(sfs_fd, n * elem_size, 0) != n * elem_size){
		perror("lseek error");
		exit(1);
	}
	if(write(sfs_fd, buf, elem_size) != elem_size){
		perror("write error");
		exit(1);
	}
}

static void read_block(uint32_t sec, void *buf)
{
	read_elem(sec, BLKSZ, buf);
}

static void write_block(uint32_t sec, void *buf)
{
	write_elem(sec, BLKSZ, buf);
}

static void read_index(uint32_t sec, void *buf)
{
	read_elem(nindex - sec - 1, IE_SIZE, buf);
}

static void write_index(uint32_t sec, void *buf)
{
	write_elem(nindex - sec - 1, IE_SIZE, buf);
}

static void init_sb(void)
{
	//read_block(0, &sb);

	sb.time_stamp = 0;
	sb.da_blk = 0;
	sb.ia_size = 0;
	sb.magic_num[0] = 'S';
	sb.magic_num[1] = 'F';
	sb.magic_num[2] = 'S';
	sb.fs_version = 0x10;	// sfs vresion 1.0

	sb.total_blk = nblk;
	sb.rev_blk = reserved_blk;     // no block for reserved
	sb.blk_size = 2;    // 512 bytes per block

	// setup checksum
	sb.checksum = 0;
	sb.checksum = -calc_checksum((char *)&sb);
	printf("sb.checksum = %d\n", sb.checksum);

	write_block(0, &sb);
}

static void update_sb(void)
{
	read_block(0, &sb);

	sb.time_stamp = 0;
	sb.da_blk = da_blocks;  // by blocks
	sb.ia_size = ia_num * IE_SIZE;

	write_block(0, &sb);
}

static void write_data(const char *name, long len, struct sfs_file *filep)
{
	int fd;
	char buf[BLKSZ];
	int n, blkstart = da_blocks;

	fd = open(name, O_RDONLY);
	if(fd < 0){
		perror(name);
		exit(1);
	}

	while(len > 0){
		memset(buf, 0, BLKSZ);
		n = (len > BLKSZ ? BLKSZ : len);
		if(read(fd, buf, n) != n){
			perror("read");
			exit(1);
		}
		write_block(BLOCK_INDEX(da_blocks), buf);
		len -= BLKSZ;
		da_blocks++;
	}

	filep->blk_start = blkstart;
	filep->blk_end = da_blocks - 1;
}

// write volumn id and starting marker entry
static void init_index(void)
{
	struct sfs_vol_id id;

	ia_num = 0;

	memset(&id, 0, IE_SIZE);
	id.etype = VOLUME_ID;
	id.time = 0;
	strcpy(id.name, "sfs-1.0");
	write_index(ia_num, &id);

	ia_num++;
	write_index(ia_num, &mark);
}

static void write_file(char *name)
{
	struct sfs_dir *dirp = NULL;
	struct sfs_file *filep = NULL;
	struct sfs_index index;
	struct stat state;
	char *p = (char *)&index;
	int len = strlen(name);
	int nie, i, j;

	// check overwrite between data area and index area

	if(stat(name, &state) == -1){
		perror("stat");
		exit(1);
	}
	switch(state.st_mode & S_IFMT){
		case S_IFDIR:
			nie = NIE_DIR(len);
			printf("%s: directory\n", name);
			dirp = (struct sfs_dir *)&index;
			break;
		case S_IFREG:
			nie = NIE_FILE(len);
			printf("%s: regular file\n", name);
			filep = (struct sfs_file *)&index;
			break;
		default:
			printf("error type\n");
			exit(1);
	}

	// move marker
	ia_num += nie + 1;
	write_index(ia_num, &mark);

	memset(&index, 0, IE_SIZE);
	if(dirp){  // for dir
		dirp->etype = DIR_ENTRY;
		dirp->ne = nie;
		dirp->time = 0;
		strncpy(dirp->name, name, DIR_SPACE);
		write_index(ia_num - 1, dirp);

		for(i = 0; i < nie; i++){
			j = ia_num - 2;
			strncpy(p, name + DIR_SPACE + IE_SIZE * i, IE_SIZE);
			write_index(j, p);
		}
	} else if(filep){  // for regular file
		write_data(name, state.st_size, filep);

		filep->etype = FILE_ENTRY;
		filep->ne = nie;
		filep->time = 0;
		filep->len = state.st_size;
		strncpy(filep->name, name, FILE_SPACE);
		write_index(ia_num - 1, filep);

		for(i = 0; i < nie; i++){
			j = ia_num - 2 - i;
			strncpy(p, name + FILE_SPACE + IE_SIZE * i, IE_SIZE);
			write_index(j, p);
		}
	}
}

int main(int argc, char *argv[])
{
	int i;

	if(argc < 2){
		printf("Usage: %s fs.img files...\n", argv[0]);
		exit(1);
	}

	assert(BLKSZ % sizeof(struct sfs_vol_id) == 0);
	assert(BLKSZ % sizeof(struct sfs_mark) == 0);
	assert(BLKSZ % sizeof(struct sfs_dir) == 0);
	assert(BLKSZ % sizeof(struct sfs_file) == 0);
	assert(BLKSZ % sizeof(struct sfs_index) == 0);

	sfs_fd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
	if(sfs_fd < 0){
		perror(argv[1]);
		exit(1);
	}

	init_sb();
	init_index();

	for(i = 2; i < argc; i++){
		//printf("file: %s\n", argv[i]);
		write_file(argv[i]);
	}

	update_sb();
	return 0;
}

