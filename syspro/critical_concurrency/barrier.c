/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#include "barrier.h"
#include <stdlib.h>
#include <stdio.h>

// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
    int err_mtx = pthread_mutex_destroy(&barrier->mtx);
    int err_cond = pthread_cond_destroy(&barrier->cv);

    if (!err_mtx || !err_cond) {
        return 1;
    }

    return 0;
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
    if (num_threads <= 0) {
        return 1;
    }
    int err_mtx = pthread_mutex_init(&barrier->mtx, NULL);
    int err_cond = pthread_cond_init(&barrier->cv, NULL);
    barrier->n_threads = num_threads;
    barrier->count = 0;
    // Zero indicates that the barrier is good to accept more threads
    barrier->times_used = 0;

    if (!err_mtx || !err_cond) {
        return 1;
    }

    return 0;
}

int barrier_wait(barrier_t *barrier) {
    pthread_mutex_lock(&barrier->mtx);
    // when the barrier is releasing threads...
    while (barrier->times_used == 1) {
        pthread_cond_wait(&barrier->cv, &barrier->mtx);
    }

    // increment the number...
    barrier->count++;

    // when the threads are stilling running into the barrier...
    if (barrier->count != barrier->n_threads) {
        while (barrier->times_used == 0 && barrier->count != barrier->n_threads) {
            pthread_cond_wait(&barrier->cv, &barrier->mtx);
        }
    } else {
        // The last thread has rushed into the barrier, and we can let them go.
        //   and we no longer receive the other threads...
        barrier->times_used = 1;
    }

    barrier->count--;
    // when the last thread has been kicked out of the barrier, we are go to
    //  receive another wave of threads...
    if (barrier->count == 0) {
        barrier->times_used = 0;
    }

    pthread_cond_broadcast(&barrier->cv);
    pthread_mutex_unlock(&barrier->mtx);

    return 0;
}
