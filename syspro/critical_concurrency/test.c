/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

static queue* que;

void* put(void* input){
    queue_push(que, input);
    fprintf(stderr, "num is: %d\n", *input);
    return NULL;
}

void* pull(void* nothing){
    void* garb = nothing;
    garb = 0;
    return queue_pull(que);
}

int main(int argc, char **argv) {
    // if (argc != 2) {
    //     printf("usage: %s test_number\n", argv[0]);
    //     exit(1);
    // }
    // printf("Please write tests cases\n");
    pthread_t pthread[100];
    pthread_t pnex[50];
    que = queue_create(30);
    long int val;
    for(val = 0; val < 100; val++){
        pthread_create(&pthread[val], NULL, &put, (void*)val);
    }
    for(val = 0; val < 50; val++){
        pthread_create(&pnex[val], NULL, &pull, NULL);
    }
    for(val = 0; val < 100; val++){
        pthread_join(pthread[val], NULL);
    }
    for(val = 0; val < 50; val++){
        int* num = NULL;
        pthread_join(pnex[val], (void*)num);
        fprintf(stderr, "num is: %d\n", *num);
        free(num);
    }
    queue_destroy(que);
    return 0;
}
