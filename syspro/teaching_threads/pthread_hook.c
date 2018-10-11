/**
* Teaching Threads Lab
* CS 241 - Fall 2018
*/

#include "pthread_hook.h"
#include <pthread.h>
#include <stdlib.h>

int __real_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine)(void *), void *arg);
int __pthread_counter = 0;
int __pthread_max_limit = -1;

void __set_pthread_max_limit(int num) {
    __pthread_max_limit = num;
}

void __reset_pthread_counter() {
    __pthread_counter = 0;
}

int __wrap_pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                          void *(*start_routine)(void *), void *arg) {
    __pthread_counter++;
    if (__pthread_max_limit >= 0) {
        if (__pthread_counter > __pthread_max_limit)
            exit(44);
    }
    return __real_pthread_create(thread, attr, start_routine, arg);
}
