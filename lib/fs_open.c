#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * fs_open - Return SYSERR if the file is already open or does not exist
 *     otherwise, add a record to the oft in the first current FSTATE_CLOSED
 *     slot corresponding to the opened file and copy the inode from the block
 *     device to the oft record and return the index into the oft table for the
 *     newly opened file.
 */
int fs_open(char* filename, int flags) {
  if(flags != O_RDONLY && flags != O_WRONLY && flags != O_RDWR) {
    printf("Invalid File Mode\n");
    return SYSERR;
  }

  struct inode node;
  int i;

  if(fsd->root_dir.numentries <= 0) {
    return SYSERR;
  }

  for(i = 0; i < fsd->root_dir.numentries; i++) {
    if(oft[i].state != FSTATE_OPEN && strcmp(fsd->root_dir.entry[i].name, filename) == 0) {
        int inode_block = fsd->root_dir.entry[i].inode_block;

        fs_get_inode_by_num(0, inode_block, &node);

        oft[i].state = FSTATE_OPEN;
        oft[i].fileptr = 0;
        oft[i].in = node;
        oft[i].de = &fsd->root_dir.entry[i];
        oft[i].flag = flags;

        fs_put_inode_by_num(0, inode_block, &oft[i].in);

        return i;
    }
  }
  return SYSERR;
}