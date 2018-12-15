/**
* Finding Filesystems Lab
* CS 241 - Fall 2018
*/

#include "minixfs.h"
#include "minixfs_utils.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

int my_min_blockcount(file_system *fs, inode *nody, int block_count) {
    data_block_number *block_array = nody->direct;
    int err = 0;
    if (block_count < NUM_DIRECT_INODES) {
        block_array = nody->direct;
        for (int i = 0; i < block_count; i++) {
            if (block_array[i] == -1) {
                err = add_data_block_to_inode(fs, nody);
                if (err == -1)
                    return -1;
                memset(fs->data_root + block_array[i], 0, sizeof(data_block));
            }
        }
    } else {
        for (int i = 0; i < NUM_DIRECT_INODES; i++) {
            if (block_array[i] == -1) {
                err = add_data_block_to_inode(fs, nody);
                if (err == -1)
                    return -1;
                memset(fs->data_root + block_array[i], 0, sizeof(data_block));
            }
        }
        err = add_single_indirect_block(fs, nody);
        if (err == -1)
            return -1;
        block_array = (data_block_number *)(fs->data_root + nody->indirect);
        block_count -= NUM_DIRECT_INODES;
        for (int i = 0; i < block_count; i++) {
            if (block_array[i] == -1) {
                err = add_data_block_to_indirect_block(fs, block_array);
                if (err == -1)
                    return -1;
                memset(fs->data_root + block_array[i], 0, sizeof(data_block));
            }
        }
    }
    return 0;
}


/**
 * Minic ceiling func in math.h
 * 9 / 5 = 2
 * 9 / 3 = 3
 */
uint64_t ceiling(uint64_t num1, uint64_t num2) {
    return num1 / num2 + ((num1 % num2) != 0);
}

/**
 * get the path of its parent.
 */
const char *get_parent_path(const char *path) {
    int len = strlen(path);
    char *ppath = strdup(path);
    for (int i = len - 2; i >= 0; i--) {
        if (ppath[i] == '/') {
            ppath[i] = '\0';
            return (const char *)ppath;
        }
    }
    return NULL;
}

uint64_t get_writing_point(file_system* fs, inode *myinode, char **wp, off_t *off) {
    // when no indirect nodes are using...check direct nodes...
    // uint64_t mysize = myinode->size;
    uint64_t offset = (uint64_t)*off;
    if (offset >= (NUM_DIRECT_INODES + NUM_INDIRECT_INODES) * sizeof(data_block)) {
        return -1;
    }
    // fprintf(stderr, "offset %zu\n", offset);
    // find on direct nodes...
    if (offset < NUM_DIRECT_INODES * sizeof(data_block)) {
        int lastBlock = offset / sizeof(data_block);
        int myoffset = offset % sizeof(data_block);
        // data_block_number currDBN = myinode->direct[lastBlock];
        // fprintf(stderr, "is: %d\n"currDBN);
        data_block *currDB = fs->data_root + myinode->direct[lastBlock];
        // data_block *currDB = fs->data_root + lastBlock;
        *wp = currDB->data + myoffset;
        // **wp = 'v';
        // fprintf(stderr, "myoffset %d\n", myoffset);

        return sizeof(data_block) - myoffset;
    }

    // fprintf(stderr, "print\n");

    uint64_t oversize = offset - NUM_DIRECT_INODES * sizeof(data_block);
    // singly indirect link only...
    int lastBlock = oversize / sizeof(data_block);
    int myoffset = oversize % sizeof(data_block);
    data_block *indirDB = fs->data_root + myinode->indirect;
    char *idptr = indirDB->data;
    data_block_number currDBN = (data_block_number)*(idptr + sizeof(data_block_number) * lastBlock);
    // fprintf(stderr, "curr dbn: %d\n", currDBN);
    data_block *currDB = fs->data_root + currDBN;
    *wp = currDB->data + myoffset;

    return 16 * KILOBYTE - myoffset;
}

/**
 * write bytes to the assigned block...
 * before calling this function, you should make sure that enough blocks are available...
 */
uint64_t write_stuff(file_system *fs, inode *myinode, const char *buf, size_t count, off_t *off) {
    char *wp = NULL;

    uint64_t tiktik = get_writing_point(fs, myinode, &wp, off);
    // if (tiktik == (uint64_t)-1) {
    //     return -1;
    // }
    uint64_t total_written = 0;
    size_t ct = 0;
    char *temp = (char *)buf;
    for (size_t i = 0; i < count; i++) {
        if (tiktik != 0) {
            wp[ct] = buf[i];
            // memcpy(wp + ct, temp, 1);
            // fprintf(stderr, "wp is: %c\n", *(wp + ct));
            total_written++;
            ct++;
            (*off)++;
            tiktik--;
            temp++;
        } else {
            // move to another direct inode...
            tiktik = get_writing_point(fs, myinode, &wp, off);
            ct = 0;
            wp[ct] = buf[i];
            // memcpy(wp + ct, temp, 1);
            total_written++;
            ct++;
            (*off)++;
            tiktik--;
            temp++;
        }
    }
    // fprintf(stderr, "wp is: %s\n", wp);

    // update size of file.
    if (myinode->size > (size_t)*off) {
        if (myinode->size < (size_t)*off + count) {
            uint64_t overlap = *off - myinode->size;
            myinode->size += (count - overlap);
        }
    } else if (myinode->size == (size_t)*off) {
        myinode->size += count;
    } else {
        uint64_t blank = myinode->size - *off;
        myinode->size += (count + blank);
    }

    return total_written;
}


uint64_t read_stuff(file_system *fs, inode *myinode, char *buf, size_t count, off_t *off) {
    char *wp = NULL;

    // fprintf(stderr, "read_stuff: count: %zu, off %ld\n", count, *off);
    uint64_t tiktik = get_writing_point(fs, myinode, &wp, off);
    uint64_t total_read = 0;
    size_t ct = 0;
    for (size_t i = 0; i < count; i++) {
        if (tiktik != 0) {
            buf[i] = *(wp + ct);
            total_read++;
            ct++;
            (*off)++;
            tiktik--;
        } else {
            // move to another direct inode...
            tiktik = get_writing_point(fs, myinode, &wp, off);
            ct = 0;
            buf[i] = *(wp + ct);
            total_read++;
            ct++;
            (*off)++;
            tiktik--;
        }
    }

    return total_read;
}

/**
 * Virtual paths:
 *  Add your new virtual endpoint to minixfs_virtual_path_names
 */
char *minixfs_virtual_path_names[] = {"info", /* add your paths here*/};

/**
 * Forward declaring block_info_string so that we can attach unused on it
 * This prevents a compiler warning if you haven't used it yet.
 *
 * This function generates the info string that the virtual endpoint info should
 * emit when read
 */
static char *block_info_string(ssize_t num_used_blocks) __attribute__((unused));
static char *block_info_string(ssize_t num_used_blocks) {
    char *block_string = NULL;
    ssize_t curr_free_blocks = DATA_NUMBER - num_used_blocks;
    asprintf(&block_string, "Free blocks: %zd\n"
                            "Used blocks: %zd\n",
             curr_free_blocks, num_used_blocks);
    return block_string;
}

// Don't modify this line unless you know what you're doing
int minixfs_virtual_path_count =
    sizeof(minixfs_virtual_path_names) / sizeof(minixfs_virtual_path_names[0]);

int minixfs_chmod(file_system *fs, char *path, int new_permissions) {
    // Thar she blows!
    errno = 0;
    inode *myinode = get_inode(fs, path);
    if (myinode == NULL) {
        errno = ENOENT;
        return -1;
    }

    // int type = (myinode->mode) >> RWX_BITS_NUMBER;
    myinode->mode = myinode->mode >> RWX_BITS_NUMBER << RWX_BITS_NUMBER | new_permissions;

    if (clock_gettime(CLOCK_REALTIME, &myinode->ctim) == -1) {
        return -1;
    }

    return 0;
}

int minixfs_chown(file_system *fs, char *path, uid_t owner, gid_t group) {
    // Land ahoy!
    errno = 0;
    inode *myinode = get_inode(fs, path);
    if (myinode == NULL) {
        errno = ENOENT;
        return -1;
    }

    if (owner != (uid_t)-1) {
        myinode->uid = owner;
    }
    if (group != (gid_t)-1) {
        myinode->gid = group;
    }

    if (clock_gettime(CLOCK_REALTIME, &myinode->ctim) == -1) {
        return -1;
    }

    return 0;
}

inode *minixfs_create_inode_for_path(file_system *fs, const char *path) {
    // Land ahoy!
    // fprintf(stdout, "touched!\n");
    errno = 0;
    // fprintf(stderr, "ops\n");
    //
    const char *temp;
    if ((temp = is_virtual_path(path)) == NULL) {
        temp = path;
    }
    // fprintf(stderr, "ops\n");
    // if we already have that inode...
    inode *tryinode = get_inode(fs, temp);
    if (tryinode != NULL) {
        // clock_gettime(CLOCK_REALTIME, &tryinode->ctim);
        return NULL;
    }

    // fprintf(stderr, "path: %s\n", path);
    //
    char *myfilename;
    inode *parent = parent_directory(fs, path, (const char **)&myfilename);
    //

    if (!valid_filename(myfilename)) {
        return NULL;
    }

    //
    inode_number myino = first_unused_inode(fs);
    if (myino == (inode_number)-1) {
        return NULL;
    }

    inode *me = fs->inode_root + myino;
    init_inode(parent, me);
    // if (clock_gettime(CLOCK_REALTIME, &parent->mtim) == -1) {
    //     return NULL;
    // }

    minixfs_dirent *mydirent = malloc(sizeof(minixfs_dirent));
    mydirent->name = myfilename;
    mydirent->inode_num = myino;

    // char *myblock = malloc(FILE_NAME_ENTRY + 1);
    // make_string_from_dirent(myblock, *mydirent);

    uint64_t parentSize = parent->size;
    off_t offset = (off_t)parentSize;
    uint64_t numOfDBNeeded = ceiling(parentSize + FILE_NAME_ENTRY, sizeof(data_block));

    // const char *ppath = get_parent_path(path);
    // fprintf(stderr, "ppath: %s\n", ppath);

    int bc_ret = my_min_blockcount(fs, parent, numOfDBNeeded);
    // fprintf(stderr, "oh? %d\n", bc_ret);

    if (bc_ret == -1) {
        free(mydirent);
        return NULL;
    } else if (bc_ret == 0) {
        // we dont need to make new block! yaaay...
        char *wp = NULL;

        get_writing_point(fs, parent, &wp, &offset);
        make_string_from_dirent(wp, *mydirent);
        parent->size += FILE_NAME_ENTRY;
    } else {
        // case that we need to break down stuff...
        // char *wp = get_writing_point(fs, parent);
        char *buf = malloc(FILE_NAME_ENTRY + 1);
        make_string_from_dirent(buf, *mydirent);

        // fprintf(stderr, "buffy is: %s\n", buf);

        // get_writing_point(fs, parent, &wp, &isDirect, &pos);
        write_stuff(fs, parent, buf, FILE_NAME_ENTRY + 1, &offset);
    }

    free(mydirent);

    return me;
}

ssize_t minixfs_virtual_read(file_system *fs, const char *path, void *buf,
                             size_t count, off_t *off) {
    if (!strcmp(path, "info")) {
        // TODO implement the "info" virtual file here
        ssize_t countUsedDB = 0;
        char *data_map = GET_DATA_MAP(fs->meta);
        for (size_t i = 0; i < DATA_NUMBER; i++) {
            if (data_map[i] == 1) {
                countUsedDB++;
            }
        }
        char *bbuf = block_info_string(countUsedDB);
        // fprintf(stderr, "it is: %d\n", countUsedDB);
        size_t sizeOfFile = strlen(bbuf);
        if (*off >= (off_t)sizeOfFile) {
            return 0;
        }
        char *track = bbuf + *off;

        size_t sizeRead = count < (sizeOfFile - *off) ? count : (sizeOfFile - *off);

        // buf = malloc(count);
        char *wp = buf;
        for (size_t i = 0; i < sizeRead; i++) {
            *(wp + i) = track[i];
        }

        *off += (off_t)sizeRead;

        return sizeRead;
    }
    // TODO implement your own virtual file here
    errno = ENOENT;
    return -1;
}

ssize_t minixfs_write(file_system *fs, const char *path, const void *buf,
                      size_t count, off_t *off) {

    // X marks the spot
    inode *myinode = get_inode(fs, path);
    // fprintf(stderr, "timestamp1\n");
    if (myinode == NULL) {
        myinode = minixfs_create_inode_for_path(fs, path);
        if (myinode == NULL) {
            errno = ENOSPC;
            return -1;
        }
    }

    if (clock_gettime(CLOCK_REALTIME, &myinode->mtim) == -1) {
        return -1;
    }
    if (clock_gettime(CLOCK_REALTIME, &myinode->atim) == -1) {
        return -1;
    }

    size_t maxsize = (NUM_DIRECT_INODES + NUM_INDIRECT_INODES) * sizeof(data_block);

    if (count >= maxsize) {
        errno = ENOSPC;
        return -1;
    }

    // buf = buf + *off;
    size_t bw = 0;
    size_t lf = count;

    while (bw != count) {
        data_block_number last_block = *off / sizeof(data_block);
        off_t offset = *off % sizeof(data_block);

        if (minixfs_min_blockcount(fs, path, (myinode->size + lf + offset) / sizeof(data_block)) == -1) {
            errno = ENOSPC;
            return -1;
        }

        // we need to use direct block
        if (last_block < NUM_DIRECT_INODES) {
            data_block *db = fs->data_root + myinode->direct[last_block];
            size_t bytes_written = sizeof(data_block) - offset;
            // fprintf(stderr, "loop: %d; bw is: %zu; left is: %zu; off is: %zu, cp: %zu, offset: %ld\n", last_block, bw, lf, *off, bytes_written, offset);
            if (bytes_written >= lf) {
                if (!memcpy(db->data + offset, buf + bw, lf)) {
                    return -1;
                }
                // buf += lf;
                myinode->size += lf;
                *off += lf;
                bw += lf;
                // fprintf(stderr, "bw: %zu\n", bw);
                return bw;
            }
            if (!memcpy(db->data + offset, buf + bw, bytes_written)) {
                return -1;
            }
            // buf += bytes_written;
            myinode->size += bytes_written;
            *off += bytes_written;
            bw += bytes_written;
            lf -= bytes_written;
        } else {
            // fprintf(stderr, "buf----: %s\n", buf + bw);
            data_block_number *arr = (data_block_number*)(fs->data_root + myinode->indirect);
            data_block_number last_iblock = last_block - NUM_DIRECT_INODES;
            data_block *db = fs->data_root + arr[last_iblock];
            size_t bytes_written = sizeof(data_block) - offset;
            // fprintf(stderr, "bytes_written: %zu\n", bytes_written);
            // fprintf(stderr, "loop: %d; bw is: %zu; left is: %zu; off is: %zu, cp: %zu, offset: %ld\n", last_iblock, bw, lf, *off, bytes_written, offset);

            if (bytes_written >= lf) {
                if (!memcpy(db->data + offset, buf + bw, lf)) {
                    return -1;
                }
                // buf += lf;
                myinode->size += lf;
                *off += lf;
                bw += lf;
                // fprintf(stderr, "bw: %zu\n", bw);
                return bw;
            }
            // fprintf(stderr, "buf: %s\n", buf + bw);
            if (!memcpy(db->data + offset, buf + bw, bytes_written)) {
                return -1;
            }
            // buf += bytes_written;
            myinode->size += bytes_written;
            *off += bytes_written;
            bw += bytes_written;
            lf -= bytes_written;
        }

    }

    return bw;
}

ssize_t minixfs_read(file_system *fs, const char *path, void *buf, size_t count,
                     off_t *off) {
    errno = 0;
    // fprintf(stdout, "wrote!\n");
    const char *virtual_path = is_virtual_path(path);
    if (virtual_path)
        return minixfs_virtual_read(fs, virtual_path, buf, count, off);

    // 'ere be treasure!
    inode *myinode = get_inode(fs, path);

    // when the file does not exist.
    if (myinode == NULL) {
        errno = ENOENT;
        return -1;
    }

    // update atim...
    if (clock_gettime(CLOCK_REALTIME, &myinode->atim) == -1) {
        return -1;
    }

    uint64_t sizeOfFile = myinode->size;

    // fprintf(stderr, "minixfs_read: path: %s, count: %zu, off %ld, size: %zu\n", path, count, *off, sizeOfFile);

    // when *off is beyond the size of file...
    if ((uint64_t)*off >= sizeOfFile) {
        // fprintf(stderr, "i am here\n");
        return 0;
    }

    // if the bytes to read are greater than the rest size of file... just read until the end reaches...
    size_t sizeToRead = count < (sizeOfFile - *off) ? count : (sizeOfFile - *off);

    // char *wp = NULL;
    uint64_t total_read = read_stuff(fs, myinode, buf, sizeToRead, off);

    return total_read;
}
