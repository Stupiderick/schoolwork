/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

// work with Yufeng Luo.

#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

static queue* que;

void* put(void* input){
    queue_push(que, input);
    fprintf(stderr, "num is: %d\n", *(int *)input);
    return NULL;
}

void* pull(void* nothing){
    void* garb = nothing;
    garb = 0;
    return queue_pull(que);
}

int main(int argc, char **argv) {
    pthread_t pthread[100];
    pthread_t pnex[50];
    que = queue_create(50);
    long int val;
    for(val = 0; val < 100; val++){
        pthread_create(&pthread[val], NULL, &put, &val);
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
