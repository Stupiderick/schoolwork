// /**
// * Critical Concurrency Lab
// * CS 241 - Fall 2018
// */
//
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
//
// #include "semamore.h"
//
// int main(int argc, char **argv) {
//     printf("Please write tests in semamore_tests.c\n");
//     Semamore *test = malloc(sizeof(Semamore));
//
//     semm_init(test, 5, 100);
//     semm_wait(test);
//     semm_post(test);
//     semm_destroy(test);
//
//     return 0;
// }


#include <stdio.h>
#include <pthread.h>
// Compile with -pthread

static int sum = 0; //shared

void *countgold(void *param) {
    int i; //local to each thread
    for (i = 0; i < 10000000; i++) {
        sum += 1;
    }
    return NULL;
}

int main() {
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, countgold, NULL);
    pthread_create(&tid2, NULL, countgold, NULL);

    //Wait for both threads to finish:
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    printf("ARRRRG sum is %d\n", sum);
    return 0;
}
