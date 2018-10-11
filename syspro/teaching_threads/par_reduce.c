/**
* Teaching Threads Lab
* CS 241 - Fall 2018
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

/* You might need a struct for each task ... */
typedef struct T {
    pthread_t id;
    int base_case;
    //int start;
    reducer reduce_func;
    size_t length;
    int *list;
    int ret;
} T;

/* You should create a start routine for your threads. */
//pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

void print_all(int *list, size_t list_len) {
    for (size_t i = 0; i < list_len; i++) {
        printf("In list: %d\n", list[i]);
    }
}

void *myReduce(void *info_in) {
    T *info = (T *)info_in;

    int base_case = info->base_case;
    //int start = info.start;
    reducer reduce_func = info->reduce_func;
    size_t length = info->length;
    int *list = info->list;    

    int result = base_case;

    //pthread_mutex_lock(&m);
    for (size_t i = 0; i < length; ++i) {
        result = reduce_func(result, list[i]);
//        printf("In info: %d\n", result);
    }
    //pthread_mutex_unlock(&m);
    info->ret = result;
    //printf("In info: %d\n", result);
    return NULL;
}

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
    /* Your implementation goes here */
    if (num_threads > list_len) {
        num_threads = list_len;
    }
    size_t norm_len = 0;
    size_t last_len = 0;
    int ret = base_case;

  //  print_all(list, list_len);

    if (list_len % num_threads != 0) {
        norm_len = list_len / (num_threads - 1);
        last_len = list_len % (num_threads - 1);
    } else {
        norm_len = list_len / num_threads;
        last_len = norm_len;
    }

    T *info = (T *)malloc(num_threads * sizeof(T));
    for (size_t i = 0; i < num_threads; i++) {
        size_t curr_len = 0;
        info[i].id = i;
        info[i].base_case = base_case;
        info[i].reduce_func = reduce_func;
        if (i != num_threads - 1) {
            info[i].list = (int *)malloc(norm_len * sizeof(int));
            curr_len = norm_len;
        } else {
            info[i].list = (int *)malloc(last_len * sizeof(int));
            curr_len = last_len;
        }
        info[i].length = curr_len;
        for (size_t j = 0; j < curr_len; j++) {
            info[i].list[j] = list[i * norm_len + j];
        }
    }

    for (size_t i = 0; i < num_threads; i++) {
        pthread_create(&info[i].id, NULL, myReduce, &info[i]);
    }

    for (size_t i = 0; i < num_threads; i++) {
        pthread_join(info[i].id, NULL);
    }
    
    for (size_t i = 0; i < num_threads; i++) {
        ret = reduce_func(ret, info[i].ret);
        free(info[i].list);
    }
   // printf("%d\n", ret);
    free(info);
    

    return ret;
}
