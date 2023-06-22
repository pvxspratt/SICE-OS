#include <xinu.h>
#include <fs.h>

fsystem_t* fsd = NULL;
filetable_t oft[NUM_FD];

int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

void fs_setmaskbit(int x) {
  if (fsd == NULL) return;
  fsd->freemask[x / 8] |= 0x1 << (x % 8);
}

void fs_clearmaskbit(int x) {
  if (fsd == NULL) return;
  fsd->freemask[x / 8] &= ~(0x1 << (x % 8));
}

int fs_getmaskbit(int x) {
  if (fsd == NULL) return -1;
  return (fsd->freemask[x / 8] >> (x % 8)) & 0x1;
}

syscall fs_mkfs(void) {
  intmask mask;
  fsystem_t fsd;
  bdev_t device = bs_stats();
  uint32 masksize, i;
  mask = disable();
  
  masksize = device.nblocks / 8;
  masksize += (device.nblocks % 8 ? 0 : 1);
  fsd.device = device;
  fsd.freemasksz = masksize;
  fsd.freemask = getmem(masksize);
  fsd.root_dir.numentries = 0;

  for (i=0; i<masksize; i++)
    fsd.freemask[i] = 0;

  for (i=0; i<DIR_SIZE; i++) {
    fsd.root_dir.entry[i].inode_block = EMPTY;
    memset(fsd.root_dir.entry[i].name, 0, FILENAME_LEN);
  }
  
  fsd.freemask[SB_BIT / 8] |= 0x1 << (SB_BIT % 8);  
  fsd.freemask[BM_BIT / 8] |= 0x1 << (BM_BIT % 8);
  bs_write(SB_BIT, 0, &fsd, sizeof(fsystem_t));
  bs_write(BM_BIT, 0, fsd.freemask, fsd.freemasksz);
  freemem(fsd.freemask, fsd.freemasksz);
  
  restore(mask);
  return OK;
}

syscall fs_mount(void) {
  intmask mask;
  int i;

  mask = disable();
  if ((fsd = (fsystem_t*)getmem(sizeof(fsystem_t))) == (fsystem_t*)SYSERR) {
    restore(mask);
    return SYSERR;
  }
  bs_read(SB_BIT, 0, fsd, sizeof(fsystem_t));
  if ((fsd->freemask = getmem(fsd->freemasksz)) == (char*)SYSERR) {
    restore(mask);
    return SYSERR;
  }
  bs_read(BM_BIT, 0, fsd->freemask, fsd->freemasksz);

  for (i=0; i<NUM_FD; i++) {
    oft[i].state = FSTATE_CLOSED;
    oft[i].fileptr = 0;
    oft[i].de = 0;
    oft[i].flag = 0;
  }
  

  restore(mask);
  return OK;
}

syscall fs_umount(void) {
  intmask mask = disable();

  bs_write(BM_BIT, 0, fsd->freemask, fsd->freemasksz);
  bs_write(SB_BIT, 0, fsd, sizeof(fsystem_t));

  freemem(fsd->freemask, fsd->freemasksz);
  freemem((char*)fsd, sizeof(fsystem_t));
  
  restore(mask);
  return OK;
}

#define INODES_PER_BLOCK (fsd->blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (((fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

/* read in an inode and fill in the pointer */
int fs_get_inode_by_num(int dev, int inode_number, struct inode *in)
{
    int bl, inn;
    int inode_off;

    if (dev != 0)
    {
        printf("Unsupported device\n");
        return SYSERR;
    }
    if (inode_number > fsd->ninodes)
    {
        printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
        return SYSERR;
    }

    bl = inode_number / INODES_PER_BLOCK;
    inn = inode_number % INODES_PER_BLOCK;
    bl += FIRST_INODE_BLOCK;

    inode_off = inn * sizeof(struct inode);

    /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

    bs_read(bl, 0, &block_cache[0], fsd->blocksz);
    memcpy(in, &block_cache[inode_off], sizeof(struct inode));

    return OK;
}

/* write inode indicated by pointer to device */
int fs_put_inode_by_num(int dev, int inode_number, struct inode *in)
{
    int bl, inn;

    if (dev != 0)
    {
        printf("Unsupported device\n");
        return SYSERR;
    }
    if (inode_number > fsd->ninodes)
    {
        printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
        return SYSERR;
    }

    bl = inode_number / INODES_PER_BLOCK;
    inn = inode_number % INODES_PER_BLOCK;
    bl += FIRST_INODE_BLOCK;

    /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

    bs_read(bl, 0, block_cache, fsd->blocksz);
    memcpy(&block_cache[(inn * sizeof(struct inode))], in, sizeof(struct inode));
    bs_write(bl, 0, block_cache, fsd->blocksz);

    return OK;
}