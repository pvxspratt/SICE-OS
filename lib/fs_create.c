#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;

/*
 * fs_create - Return SYSERR if not enough space is available
 *     in the directory or the filename already exists.
 *     Otherwise:
 *        1. Find an available block on the block store
 *        2. Create an inode_t for the new file
 *        3. Update the root directory
 *        4. Write the inode and free bitmask back to the block device
 */
syscall fs_create(char* filename) {
    int m;
  if(m == O_CREAT && fsd->root_dir.numentries < DIR_SIZE) {

    for(int i = 0; i < fsd->root_dir.numentries; i++) {
        if(strcmp(filename, fsd->root_dir.entry[i].name) == 0) {
            return SYSERR;
        }
    }

    struct inode in;

    int get_node_status = fs_get_inode_by_num(0, ++fsd->inodes_used, &in);

    in.id = fsd->inodes_used;
    in.type = INODE_TYPE_FILE;
    in.device = 0;
    in.type = INODE_TYPE_FILE;
    in.size = 0;

    int put_inode_status = fs_put_inode_by_num(0, in.id, &in);

    strcpy(fsd->root_dir.entry[fsd->root_dir.numentries].name, filename);
    fsd->root_dir.entry[fsd->root_dir.numentries].inode_block = in.id;

    fsd->root_dir.numentries++;

    return fs_open(filename, O_RDWR);
  }
  
  return SYSERR;
}