/**
* Mini Valgrind Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>

int main() {
    // Your tests here using malloc and free

    int *a = malloc(100);
    int *b = malloc(500);
//    b = realloc(b, 600);
    int *c = malloc(1000);
    c = realloc(c, 10000);
//    int c = 50;
//    int *d = &c;
    free(b);
//    puts("hello\n");
   free(a);
//    puts("what?\n");
    free(c);
//    puts("yes\n");

    return 0;
}

