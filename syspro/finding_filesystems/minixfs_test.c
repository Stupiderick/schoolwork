/**
* Finding Filesystems Lab
* CS 241 - Fall 2018
*/

#include "minixfs.h"
#include "minixfs_utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // Write tests here!
    file_system *fs = open_fs("test.fs");
    inode *root = get_inode(fs, "/");
    printf("%p, %p, %d\n", root, fs->inode_root, is_directory(root));

    inode *r1 = get_inode(fs, "/goodies");
    if (r1 == NULL) {
        fprintf(stderr, "no r1\n");
    } else {
        fprintf(stderr, "r1 mode: %hu\n", r1->mode);
    }

    inode *r2 = get_inode(fs, "/goodies/bbb.txt");
    if (r2 == NULL) {
        fprintf(stderr, "no r2\n");
    } else {
        fprintf(stderr, "r2 mode: %hu\n", r2->mode);
    }

    inode *r3 = minixfs_create_inode_for_path(fs, "/bbb.txt");
    if (r3 == NULL) {
        fprintf(stderr, "no r3\n");
    } else {
        fprintf(stderr, "r3 mode: %hu\n", r3->mode);
    }

    char *buf = malloc(5);
    off_t off = 0;
    minixfs_write(fs, "/bbb.txt", "woof", 5, &off);
    off = 0;
    minixfs_read(fs, "/bbb.txt", buf, 5, &off);
    fprintf(stderr, "buf is: %s\n", buf);

    free(buf);

    return 0;
}
