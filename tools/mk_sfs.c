#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>

#include "mk_sfs.h"

#ifdef NUMBLK
#define NBLK	NUMBLK
#else
#define NBLK	(170 * 16 * 63)  // hd size ~= 80M with sb.blk_size=2
#endif

#define BLKSZ	512
#define NAME_MAX     255   // # chars in a file name
#define BLOCK_INDEX(da_num) ((da_num) + reserved_blk)
#define ROOT_INODE 1

int sfs_fd;
struct sfs_superblock sb;
uint32_t nblk = NBLK;
uint32_t nindex = NBLK * (BLKSZ / IE_SIZE);
uint32_t reserved_blk = 1;     // no block for reserved
uint32_t da_blocks = 0;  // data area size in blocks, as free block index start from 0
uint32_t ia_num = 0;  // index area size = ia_num * IE_SIZE, and pointed to START_MARK
struct sfs_mark mark = { START_MARK, };
static int root_ia_num = 1; // root node ('/') in index area always is 1, after mark node

#define NIBUF	32
static struct sfs_inode ibuf[NIBUF];  // static variable contains all zeros


static void inc_and_verify(uint32_t *var, int n)
{
	*var += n;
	if(da_blocks + ((ia_num + 1)/8 + 1) >= nblk){
		printf("Error! No free space left!\n");
		exit(0);
	}
}

static char calc_checksum(char *sb)
{
	char sum = 0;
	int cnt;

	for(cnt = 0x1ac; cnt <= 0x1bd; cnt++)
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

// inode process
// return the index of free position in ibuf or -1 if no slot available
static int sfs_inode_alloc(void)
{
	int i;

	for(i = 0; i < NIBUF; i++){
		if(ibuf[i].flags == 0)
			break;
	}
	return (i >= NIBUF ? -1 : i);
}

// return the index of ibuf whose ni is equal to the searched ni.
// return -1 if not found
static int sfs_inode_search(int ni)
{
	int i;

	for(i = 0; i < NIBUF; i++){
		if(ibuf[i].ni == ni)
			break;
	}
	return (i >= NIBUF ? -1 : i);
}

void sfs_init_inode(void)
{
	// nothing need to be done,since ibuf[] should be NULL already
}

// return j
struct sfs_inode *sfs_inode_load(int ino)
{
	int i;
	struct sfs_inode *ip;

	i = sfs_inode_search(ino);
	if(i >= 0)
		return &ibuf[i]; // TODO: check flag

	i = sfs_inode_alloc();
	if(i == -1)
		return NULL;

	ip = &ibuf[i];
	ip->ni = ino;
	ip->nref = 1;
	ip->flags = SFS_INODE_INUSE;
	read_index(ino, &ip->sindex);

	return ip;
}

void sfs_inode_free(int ino)
{
	return;
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
	sb.rev_blk = reserved_blk;
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
		inc_and_verify(&da_blocks, 1);
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

	inc_and_verify(&ia_num, 1);
	write_index(ia_num, &mark);
}

// create directory items '.' and '..' located at block blk_num,
// '..' pointed to index area inode number 'parent_inum'.
// return the length of bytes taked by them.
int create_dot_dir_items(int parent_inum, int self_inum, int blk_num)
{
	char buf[BLKSZ];
	struct sfs_dirent sdirent;
	char *p = (char *)&sdirent;
	char *self = ".";
	char *parent = "..";
	int len = 0;

	memset(buf, 0, BLKSZ);

	// process '.'
	sdirent.len = DIRENT_LEN(strlen(self));
	sdirent.ino = self_inum;
	sdirent.name = self;
	memmove(buf, p, 8); // len+ino = 8 bytes
	memmove(buf+8, sdirent.name, strlen(self));
	len += sdirent.len;

	// process '..'
	sdirent.len = DIRENT_LEN(strlen(parent));
	sdirent.ino = parent_inum;
	sdirent.name = parent;
	memmove(buf+len, p, 8); // len+ino = 8 bytes
	memmove(buf+len+8, sdirent.name, strlen(self));
	len += sdirent.len;

	return len;
}

struct sfs_inode *search_dir(struct sfs_inode *ip, char *name)
{
	struct sfs_dir *dp;
	struct sfs_dirent *drtp;
	char *buf, *p = buf;
	int len, i = 0;
	int namelen = strlen(name);

	dp = (struct sfs_dir *)&(ip->sindex);

	if(dp->etype != DIR_ENT){
		printf("Error! not a dir\n");
		return NULL;
	}

	len = dp->len;
	buf = malloc(len);
	while(len > 0){
		read_block(dp->blk_start + i, p);
		i++;
		len -= BUFSIZ;
		p += BUFSIZ;
	}

	p = buf;
	len = dp->len;
	while(len > 0){
		drtp = (struct sfs_dirent *)p;
		len -= drtp->len;

		if(drtp->len - 9 != namelen)
			continue;
		if(strcmp(name, drtp->name) == 0){ // find now
			return sfs_inode_load(drtp->ino);
		}
	}

	return NULL;
}

// search path, skip leading '/', save path's first element to 'name',
// return the start of following element without any leading '/'.
static char* path_down(char *path, char *name)
{
	char *pp;
	int len;

	pp = path;
	while(*pp == '/')
		pp++;
	if(*pp == 0)
		return 0;

	path = pp;
	while(*pp != '/' && *pp != 0)
		pp++;
	len = pp - path;
	if(len >= NAME_MAX){
		memmove(name, path, NAME_MAX);
	} else {
		memmove(name, path, len);
		name[len] = 0;
	}

	while(*pp == '/')
		pp++;

	return pp;
}

// name to sfs_inode translation
struct sfs_inode *namei(char *path)
{
	char name[NAME_MAX];
	struct sfs_inode *ip, *next;

	if(*path == '/')
		ip = sfs_inode_load(ROOT_INODE);
	//	ip = iget(ROOTDEV, ROOTINO);
	//else
	//	ip = idup(current->cwd);
	//dump_inode(ip);

	while((path = path_down(path, name)) != 0){
		printf("namei: path: %s, name: %s\n", path, name);
		if(ip->sindex.etype != DIR_ENT){
			printf("ip(ino = %d) isn't a dir\n", ip->ni);
			return NULL;
		}

		if((next = search_dir(ip, name)) == NULL){
			printf("file not found\n");
			return NULL;
		}

		ip = next;
	}
	return ip;
}

// init '/', makes both '..' and '.' point to itself
void init_root(void)
{
	struct sfs_dir sdir;

	memset(&sdir, 0, IE_SIZE);
	sdir.etype = DIR_ENT;
	sdir.ne = 0;
	sdir.time = 0;
	sdir.blk_start = da_blocks;
	sdir.blk_end = da_blocks;
	sdir.len = create_dot_dir_items(root_ia_num, root_ia_num, da_blocks);

	strcpy(sdir.name, "/");
	write_index(ia_num, &sdir);

	// move boundary
	inc_and_verify(&da_blocks, 1);
	inc_and_verify(&ia_num, 1);
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
	inc_and_verify(&ia_num, nie + 1);
	write_index(ia_num, &mark);

	memset(&index, 0, IE_SIZE);
	if(dirp){  // for dir
		dirp->etype = DIR_ENT;
		dirp->ne = nie;
		dirp->time = 0;
		strncpy(dirp->name, name, DIR_SPACE);
		write_index(ia_num - 1, dirp);

		for(i = 0; i < nie; i++){
			j = ia_num - 2 - i;
			strncpy(p, name + DIR_SPACE + IE_SIZE * i, IE_SIZE);
			write_index(j, p);
		}
	} else if(filep){  // for regular file
		write_data(name, state.st_size, filep);

		filep->etype = FILE_ENT;
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

// copy path/to/file to sfs /path/to/file
// e.g.: host: doc/test ==> chunix: /doc/test
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
	init_root();

	for(i = 2; i < argc; i++){
		//printf("file: %s\n", argv[i]);
		write_file(argv[i]);
	}

	update_sb();
	return 0;
}

