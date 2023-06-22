#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];


/*
 * fs_close - Return SYSERR if the file is already closed
 *     otherwise, write the file's inode back to the block
 *     device and set the entry in the oft to FSTATE_CLOSED
 */
syscall fs_close(int fd) {
  if(oft[fd].state == FSTATE_CLOSED) {
    printf("State is closed already.\n");
    return SYSERR;
  } else if(fd < 0 || fd >= NUM_FD) {
    printf("File Invalid\n");
    return SYSERR;
  }

  oft[fd].state = FSTATE_CLOSED;
  oft[fd].fileptr = 0;
    
  return OK;
}