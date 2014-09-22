#include <fs.h>


struct fs_node *fs_root;	// the root of the file system

void open_fs(struct fs_node *node, int flag)
{
	if(node->open)
		return node->open(node, flag);
}

void close_fs(struct fs_node *node)
{
	if(node->close)
		return node->close(node);
}

uint32_t read_fs(struct fs_node *node, uint32_t offset, uint32_t size, char *buffer)
{
	if(node->read)
		return node->read(node, offset, size, buffer);
	else
		return 0;
}

uint32_t write_fs(struct fs_node *node, uint32_t offset, uint32_t size, char *buffer)
{
	if(node->write)
		return node->write(node, offset, size, buffer);
	else
		return 0;
}

struct dirent *readdir_fs(struct fs_node *node, uint32_t index)
{
	if(node->readdir && ((node->flags & 0xf) == FS_DIR))
		return node->readdir(node, index);
	else
		return 0;
}

struct fs_node *finddir_fs(struct fs_node *node, char *name)
{
	if(node->finddir && ((node->flags & 0xf) == FS_DIR))
		return node->finddir(node, name);
	else
		return 0;
}
