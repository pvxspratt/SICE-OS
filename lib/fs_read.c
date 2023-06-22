#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * Copy the data from the block device into the 'buff' argument
 *    Use the fileptr field of the oft to track the location of the next read/write
 *    operation.
 *    - Determine the current block and offset from the fileptr
 *    - Copy bytes into the 'buff' until you have exhausted the block or copied 'len' bytes
 *    - Repeat until 'len' bytes are copied
 *    - Update the fileptr field
 *    - Return the number of bytes read
 */
int fs_read(int fd, char* buff, int len) {
    int i, bytes;
    struct filetable ft = oft[fd];
    struct inode node = ft.in;
    int inode_block = (ft.fileptr / fsd->blocksz);
    int offset = (ft.fileptr % fsd->blocksz);
    int nnbytes = len;

    if (oft[fd].state == FSTATE_CLOSED)
    {
        printf("State is closed already.\n");
        return SYSERR;
    }

    else if (len == 0)
    {
        printf("Empty File\n");
        return SYSERR;
    }

    else if (fd < 0 || fd > NUM_FD)
    {
        printf("File Invalid\n");
        return SYSERR;
    }

    if (inode_block < INODE_BLOCKS)
    {
        int in_blk = ft.in.blocks[inode_block];
        int new_blocks = fsd->blocksz - offset;

        while (len > 0)
        {
            if (new_blocks <= len)
            {
                if (inode_block == INODE_BLOCKS - 1)
                {
                    printf("Size exceeded\n");
                    bytes = nnbytes - len;
                    return bytes;
                }

                bs_read(in_blk, offset, buff, new_blocks);

                ft.fileptr = ft.fileptr + new_blocks;
                buff = buff + new_blocks;
                len = len - new_blocks;

                memcpy(oft + fd, &ft, sizeof(struct filetable));
                in_blk = node.blocks[++inode_block];
                offset = 0;
            }
            else if (new_blocks > len)
            {
                bs_read(in_blk, offset, buff, len);
                buff = buff + len;
                ft.fileptr = ft.fileptr + len;
                len = 0;
                return nnbytes;
            }
        }
    }
    bytes = nnbytes - len;
    return bytes;
}