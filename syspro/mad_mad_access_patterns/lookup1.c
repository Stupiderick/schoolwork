/**
* Mad Mad Access Patterns Lab
* CS 241 - Fall 2018
*/

// partnered with luo34 and yuyunli2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tree.h"
#include "utils.h"


int seeker(FILE *myfile, uint32_t offset, const char *targetWord) {
    fseek(myfile, offset, SEEK_SET);

    if (feof(myfile)) {
		return -1;
	}

    char mychar;
    size_t strsize = 0;

    fseek(myfile, sizeof(BinaryTreeNode), SEEK_CUR);

    while (1) {
        if ((mychar = fgetc(myfile)) == '\0') {
            break;
        }
        if (mychar == EOF) {
            return -1;
        }
        strsize++;
        // fprintf(stderr, "char is: %c, pos: %d\n", mychar, pos);
    }
    // strsize++;
    // fprintf(stderr, "char is: %c, strsize: %zu\n", mychar, strsize);
    char currWord[strsize + 1];

    fseek(myfile, offset + sizeof(BinaryTreeNode), SEEK_SET);
    for (size_t i = 0; i < strsize; i++) {
        currWord[i] = (char)fgetc(myfile);
    }
    currWord[strsize] = '\0';

    fseek(myfile, offset, SEEK_SET);
    // fread(buf, strsize, 1, myfile);

    // fprintf(stderr, "buf is: %s\n", buf);

    BinaryTreeNode *node = malloc(sizeof(BinaryTreeNode) + strsize + 1);
    fread(node, sizeof(BinaryTreeNode), 1, myfile);
    // char *currWord = node->word;
    // fprintf(stderr, "strsize: %zu; word: %s\n", strsize, currWord);
    // fprintf(stderr, "currWord: %s\n", node->word);

    int cmpRes = strcmp(targetWord, currWord);
    // free(currWord);

    if (!cmpRes) {
        printFound(targetWord, node->count, node->price);
        free(node);
        return 0;
    } else if (cmpRes < 0) {
        if (node->left_child > 0) {
            // printf("left_child is: %d\n", node->left_child);
            int lc = seeker(myfile, node->left_child, targetWord);
            if (lc == 0) {
                free(node);
                return 0;
            } else if (lc == -1) {
                free(node);
                return -1;
            }
        } else {
            printNotFound(targetWord);
            free(node);
            return 0;
        }
    } else {
        if (node->right_child > 0 && cmpRes > 0) {
            // printf("right_child is: %d\n", node->right_child);
            int rc = seeker(myfile, node->right_child, targetWord);
            if (rc == 0) {
                free(node);
                return 0;
            } else if (rc == -1) {
                free(node);
                return -1;
            }
        } else {
            printNotFound(targetWord);
            free(node);
            return 0;
        }
    }

    return 0;
}

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

int main(int argc, char **argv) {
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }

    FILE *myfile = fopen(argv[1], "r");
    if (myfile == NULL) {
        openFail((const char *)argv[1]);
        exit(2);
    }

    // check if the first four chars are BTRE..
    fseek(myfile, 0, SEEK_SET);
    char firstFour[5];
    fread(firstFour, 4, 1, myfile);

    firstFour[4] = '\0';

    if (strcmp(firstFour, "BTRE")) {
        formatFail((const char *)argv[1]);
        // free(firstFour);
        fclose(myfile);
        exit(2);
    }
    // free(firstFour);

    // start searching
    for (int i = 2; i < argc; i++) {
        int res = seeker(myfile, 4, argv[i]);
        if (res == -1) {
            formatFail((const char *)argv[1]);
            fclose(myfile);
            exit(2);
        }
    }
    fclose(myfile);

    return 0;
}
