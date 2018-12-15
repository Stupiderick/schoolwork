/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

// partnered with luo34 and yuyunli2

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "tree.h"
#include "utils.h"

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses mmap to access the data.

  ./lookup2 <data_file> <word> [<word> ...]
*/

static void *head;

int seeker(void *nodeHead, char *targetWord) {
    // fseek(myfile, pos, SEEK_SET);
    char *strReader = (char *)nodeHead + sizeof(BinaryTreeNode);
    // char mychar;
    // size_t strsize = 0;

    while (1) {
        if (*strReader == '\0') {
            break;
        }
        // fprintf(stderr, "char is: %c, pos: %d\n", mychar, pos);
        if (*strReader == EOF) {
            return -1;
        } else {
            // strsize++;
            strReader++;
        }
    }
    // strsize++;
    // fprintf(stderr, "char is: %c, strsize: %zu\n", mychar, strsize);

    // char *buf = malloc(strsize);
    // strReader = (char *)nodeHead + sizeof(BinaryTreeNode);
    // char *buf = strdup(strReader);

    BinaryTreeNode *node = (BinaryTreeNode *)nodeHead;
    char *currWord = node->word;
    // fprintf(stderr, "currWord: %s\n", node->word);

    if (!strcmp(targetWord, currWord)) {
        printFound(targetWord, node->count, node->price);
        // free(buf);
        return 1;
    }

    if (node->left_child != 0 && strcmp(targetWord, currWord) < 0) {
        // printf("left_child is: %d\n", node->left_child);
        int lc = seeker(head + node->left_child, targetWord);
        if (lc == 1) {
            // free(buf);
            return 1;
        } else if (lc == -1) {
            // free(buf);
            return -1;
        }
    }

    if (node->right_child != 0 && strcmp(targetWord, currWord) > 0) {
        // printf("right_child is: %d\n", node->right_child);
        int rc = seeker(head + node->right_child, targetWord);
        if (rc == 1) {
            // free(buf);
            return 1;
        } else if (rc == -1) {
            // free(buf);
            return -1;
        }
    }
    // free(buf);

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }

    // FILE *myfile = fopen(argv[1], "r+");
    // if (myfile == NULL) {
    //     openFail((const char *)argv[1]);
    //     exit(2);
    // }
    //
    // int fd = fileno(myfile);

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        openFail((const char *)argv[1]);
        exit(2);
    }

    struct stat mystat;
    if (fstat(fd, &mystat) == -1) {
        openFail((const char *)argv[1]);
        // fclose(myfile);
        exit(2);
    }

    size_t sizeOfFile = mystat.st_size;
    // fprintf(stderr, "size of file: %zu\n", sizeOfFile);

    // lseek(fd, sizeOfFile, SEEK_SET);

    void *mappedHead = mmap(NULL, sizeOfFile, PROT_READ, MAP_SHARED, fd, 0);
    if (mappedHead == (void *)-1) {
        mmapFail((const char *)argv[1]);
        exit(1);
    }
    // fprintf(stderr, "mapped at %p\n", mappedHead);

    char firstFour[5];
    char *reader = (char *)mappedHead;
    head = mappedHead;
    for (size_t i = 0; i < 4; i++) {
        firstFour[i] = reader[i];
    }
    firstFour[4] = '\0';
    if (strcmp(firstFour, "BTRE")) {
        formatFail((const char *)argv[1]);
        // free(firstFour);
        // fclose(myfile);
        exit(2);
    }
    // free(firstFour);

    // move reader to the start of the first node...

    reader += 4;


    for (int i = 2; i < argc; i++) {
        int res = seeker((void *)reader, argv[i]);
        if (res == -1) {
            formatFail((const char *)argv[1]);
            // fclose(myfile);
            munmap(mappedHead, sizeOfFile);
            exit(2);
        } else if (res == 0) {
            printNotFound(argv[i]);
        }
    }

    // fclose(myfile);
    munmap(mappedHead, sizeOfFile);

    return 0;
}
