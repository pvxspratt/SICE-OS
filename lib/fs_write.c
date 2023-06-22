#include <xinu.h>
#include <fs.h>

extern fsystem_t* fsd;
extern filetable_t oft[NUM_FD];

/*
 * Copy 'len' bytes from 'buff' into the blocks associated with the file 'fd'
 *   - Until 'len' bytes are copied...
 *   - Check if a new block is needed (find free block and add to the file's blocks if necessary)
 *   - Write bytes from the 'buff' into the block starting at the fileptr
 *   - Write 'len' bytes or fill the block, whichever is less
 *   - Repeat until write is complete
 *   - Update inode state to reflect changes
 *   - Return the number of bytes written
 */
int fs_write(int fd, char* buff, int len) {
    int i, bytes;
    struct filetable ft = oft[fd];
    struct inode node = ft.in;
    int offset = (ft.fileptr % fsd->blocksz);
    int inode_block = (ft.fileptr / fsd->blocksz);
    int new_block = fsd->blocksz - offset;
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

    else if (inode_block < INODE_BLOCKS)
    {
        while (len > 0)
        {
            if (node.blocks[inode_block] > 0)
            {
                i = node.blocks[inode_block];
            }
            else if (node.blocks[inode_block] == 0)
            {
                int blocks, j;
                blocks = fsd->nblocks;

                for (j = 0; j < blocks; j++)
                {
                    if (fs_getmaskbit(j) == 0)
                    {
                        i = j;
                    }
                }

                memcpy(node.blocks + inode_block, &i, sizeof(int));
                memcpy(&((oft + fd)->in), &(node), sizeof(struct inode));

                ft.in = node;

                fs_put_inode_by_num(0, node.id, &node);
                fs_setmaskbit(i);
            }

            if (new_block > len)
            {
                bs_write(i, offset, buff, len);

                ft.fileptr = ft.fileptr + len;
                len = 0;

                memcpy(oft + fd, &ft, sizeof(struct filetable));
                return nnbytes;
            }
            if (new_block <= len)
            {
                if (inode_block == INODE_BLOCKS - 1)
                {
                    printf("Size Exceeded\n");
                    bytes = nnbytes - len;
                    return bytes;
                }

                bs_write(i, offset, buff, new_block);
                buff = buff + new_block;
                len = len - new_block;
                ft.fileptr = ft.fileptr + new_block;

                memcpy(oft + fd, &ft, sizeof(struct filetable));

                inode_block++;
                offset = 0;
            }
        }
    }

    bytes = nnbytes - len;
    return bytes;
}