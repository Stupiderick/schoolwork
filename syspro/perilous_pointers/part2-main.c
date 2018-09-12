/**
* Pointers Gone Wild Lab
* CS 241 - Spring 2018
*/

#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
    // your code here
    // p1
    first_step(81);
    
    // p2
    int v21 = 132;
    int *v22 = &v21;
    second_step(v22);

    // p3
    int v31 = 8942;
    int *v32 = &v31;
    int **v33 = &v32;
    double_step(v33);

    // p4
    char *v4 = "hello\17\0\0\0\0";
    strange_step(v4);

    // p5
    void *v5 = "hel\0";
    empty_step(v5);

    // p6
    void *v61 = "lieu";
    char *v62 = (char *)v61;
    two_step(v61, v62);

    // p7
    char *v71 = "hellothere";
    char *v72 = v71 + 2;
    char *v73 = v72 + 2;
    three_step(v71, v72, v73);

    // p8
    char *v81 = "aaaa";
    char *v82 = "iiii";
    char *v83 = "qqqq";
    step_step_step(v81, v82, v83);

    // p9
    char *v91 = "\nyyy";
    int v92 = 10;
    it_may_be_odd(v91, v92);

    // 10
    char v101[] = "CS241,CS241,CS241";
    tok_step(v101);

    // 11
    void *v111 = "\1\2\0\0\0";
    void *v112 = v111;
    the_end(v111, v112);

    return 0;
}
