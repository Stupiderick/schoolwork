/**
* Extreme Edge Cases Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camelCaser.h"
#include "camelCaser_tests.h"

/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @param  destroy      A pointer to the function that destroys camelCaser
 * output.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *),
                    void (*destroy)(char **)) {
    // Return 1 if the passed in function works properly; 0 if it doesn't.
    // Case 0: input NULL
    char **test0 = camelCaser(NULL);
    if (test0 != NULL) {
        printf("failed test0\n");
        return 0;
    }

    char **test00 = camelCaser("");
    if (test00[0] != NULL) {
        printf("failed test00\n");
        destroy(test00);
        return 0;
    }
    destroy(test00);


    // Case 1: normal input "i love champaign."
    char **test1 = camelCaser("I love champaign, And Urbana. LOL.");
    if (strcmp(test1[0], "iLoveChampaign") != 0 || strcmp(test1[1], "andUrbana") != 0 || strcmp(test1[2], "lol") != 0 || test1[3] != NULL) {
        printf("failed test1\n");
        printf("%s\n", test1[0]);
        destroy(test1);
        return 0;
    }
    destroy(test1);

    // Case 2: single dot "."
    char **test2 = camelCaser(".");
    if (strlen(test2[0]) != 0 || test2[1] != NULL) {
         printf("failed test2\n");
         destroy(test2);
         return 0;
    }
    destroy(test2);


    // Case 3: white space
    char **test3 = camelCaser("hello      world,     .");
    if (strcmp(test3[0], "helloWorld") != 0 || strlen(test3[1]) != 0 || test3[2] != NULL) {
        printf("failed test3\n");
        destroy(test3);
        return 0;
    }
    destroy(test3);

    // Case 4: weird case
    char **test4 = camelCaser("HELLO WORLD\" THIS\tiS\nErIcK. hI.");
    if (strcmp(test4[0], "helloWorld") || strcmp(test4[1], "thisIsErick") || strcmp(test4[2], "hi") || test4[3] != NULL) {
        printf("failed test4\n");
        destroy(test4);
        return 0;
    }
    destroy(test4);


    // Case 5: ASCII
    char **test5 = camelCaser("\t\n\x04\x09.");
    //printf("%s\n", test5[0]);
    if (strcmp(test5[0], "\x04") != 0 || test5[1] != NULL) {
        printf("failed test5\n");
        destroy(test5);
        return 0;
    }
    destroy(test5);

    // Case 6: check punctuations
    char **test6 = camelCaser("!@#$%^&*()_+,.<>:'?");
    if (strlen(test6[0]) || strlen(test6[1]) || strlen(test6[2]) || strlen(test6[3]) || strlen(test6[4]) || strlen(test6[5]) ||
        strlen(test6[6]) || strlen(test6[7]) || strlen(test6[8]) || strlen(test6[9]) || strlen(test6[10]) || strlen(test6[11]) ||
        strlen(test6[12]) || strlen(test6[13]) || strlen(test6[14]) || strlen(test6[15]) || strlen(test6[16]) || strlen(test6[17]) ||
        strlen(test6[18]) || test6[19] != NULL ) {
         printf("failed test6\n");
         destroy(test6);
         return 0;
    }
    destroy(test6);

    // Case 7: check numbers
    char **test7 = camelCaser("123 456. 567 abc. yesssssss");
    if (strcmp(test7[0], "123456") != 0 || strcmp(test7[1], "567Abc") != 0 || test7[2] != NULL) {
        printf("failed test7\n");
        printf("%s\n", test7[1]);
        destroy(test7);
        return 0;
    }
    destroy(test7);

    // Case 8: still weird...
    char **test8 = camelCaser("\0fidnviaebg.");
    if (test8[0] != NULL) {
        printf("failed test8\n");
        destroy(test8);
        return 0;
    }
    destroy(test8);

    char **test9 = camelCaser("123hello  123abt.");
    if (strcmp(test9[0], "123hello123Abt") != 0 || test9[1] != NULL) {
        printf("%s\n", test9[0]);
        printf("failed test9\n");
        destroy(test9);
        return 0;
    }
    destroy(test9);

    char **test10 = camelCaser("hello.       world. hello        world.");
    if (strcmp(test10[0], "hello") != 0 || strcmp(test10[1], "world") != 0 || strcmp(test10[2], "helloWorld") != 0 || test10[3] != NULL) {
        printf("failed test10\n");
        destroy(test10);
        return 0;
    }
    destroy(test10);

    char **test11 = camelCaser("689is 1virus.777  is\na\ttrash.");
    if (strcmp(test11[0], "689is1Virus") != 0 || strcmp(test11[1], "777IsATrash") != 0 || test11[2] != NULL) {
        printf("failed test11\n");
        destroy(test11);
        return 0;
    }
    destroy(test11);

    // eventually return SUCCEED
    return 1;
}
