#include <const.h>
#include <fs.h>
#include <buf.h>
#include <ext2.h>
#include <printf.h>


extern enum fs_type root_fs_type;

static struct super_block *initrd_read_super(struct super_block *sb)
{
	printk("In initrd_read_super\n");

	if(root_fs_type != INITRD)
		return NULL;

	printk("Enter initrd_read_super now\n");
	return sb;
}

struct fs_node initrd_ext2_fs = {
	initrd_read_super,
	"initrd",
	1,	// set to 1 temporarily
	NULL,
};

void add_initrd_fs(void)
{
	register_filesystem(&initrd_ext2_fs);
}

void del_initrd_fs(void)
{
	unregister_filesystem(&initrd_ext2_fs);
}

void check_initrd_fs(void)
{
	int i;
	struct buf *sbuf;
	struct ext2_super_block sb;
	char *sbp = &sb;

	printk("do some initrd check...\n");
	sbuf = bread(MKDEV(MEM_MAJOR, 0), 2);
	memmove(sbp, sbuf->data, 512);
	brelse(sbuf);

	sbuf = bread(MKDEV(MEM_MAJOR, 0), 3);
	memmove(sbp+512, sbuf->data, 512);
	brelse(sbuf);

	// check ext2 magic number
	printk("magic: %x\n", sb.magic);
	if(sb.magic == EXT2_SUPER_MAGIC){
		printk("This initrd fs is an ext2 fs\n");
		dump_ext2_superblock(&sb);
	}
}
