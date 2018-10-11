/**
* Critical Concurrency Lab
* CS 241 - Fall 2018
*/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
typedef int (*pthread_create_fn)(pthread_t *thread, const pthread_attr_t *attr,
                                 void *(*start_routine)(void *), void *arg);
typedef int (*pthread_join_fn)(pthread_t thread, void **retval);
typedef int (*pthread_cancel_fn)(pthread_t thread);
typedef void (*pthread_exit_fn)(void *retval);
typedef int (*pthread_cond_wait_fn)(pthread_cond_t *restrict cond,
                                    pthread_mutex_t *restrict mutex);
typedef int (*pthread_cond_timedwait_fn)(
    pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex,
    const struct timespec *restrict abstime);
typedef int (*pthread_cond_broadcast_fn)(pthread_cond_t *restrict cond);
typedef int (*pthread_cond_signal_fn)(pthread_cond_t *restrict cond);
typedef int (*pthread_cond_destroy_fn)(pthread_cond_t *restrict cond);

static pthread_create_fn orig_pthread_create;
static pthread_join_fn orig_pthread_join;
static pthread_cancel_fn orig_pthread_cancel;
static pthread_exit_fn orig_pthread_exit;
static pthread_cond_wait_fn orig_pthread_cond_wait;
static pthread_cond_timedwait_fn orig_pthread_cond_timedwait;
static pthread_cond_broadcast_fn orig_pthread_cond_broadcast;
static pthread_cond_signal_fn orig_pthread_cond_signal;
static pthread_cond_destroy_fn orig_pthread_cond_destroy;

static int MAX_THREADS = 8;
static int count;

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "%s [-m MAX_THREADS] <command> [command_args]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!strcmp(argv[1], "-m")) {
        MAX_THREADS = atoi(argv[2]);
        argv += 2;
    }

    char **env = malloc(3 * sizeof(char *));
    env[0] = NULL;
    env[1] = NULL;
    asprintf(&env[0], "LD_PRELOAD=./pthread_hook.so");
    asprintf(&env[1], "MAX_THREADS=%d", MAX_THREADS);
    env[2] = NULL;

    execve(argv[1], argv + 1, env);
    perror("Exec failed");
    free(env[0]);
    free(env[1]);
    free(env);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg) {
    if (!orig_pthread_create) {
        fprintf(stderr, "Unable to get original pthread_create\n");
        exit(EXIT_FAILURE);
    }
    if (++count >= MAX_THREADS) {
        fprintf(stderr, "Thread Limit Exceeded %d %d\n", count, MAX_THREADS);
        exit(-3);
    }
    return orig_pthread_create(thread, attr, start_routine, arg);
}

int pthread_join(pthread_t thread, void **retval) {
    if (!orig_pthread_join) {
        fprintf(stderr, "Unable to get original pthread_join\n");
        exit(EXIT_FAILURE);
    }
    count--;
    return orig_pthread_join(thread, retval);
}

int pthread_cancel(pthread_t thread) {
    if (!orig_pthread_cancel) {
        fprintf(stderr, "Unable to get original pthread_cancel\n");
        exit(EXIT_FAILURE);
    }
    count--;
    return orig_pthread_cancel(thread);
}

void pthread_exit(void *retval) {
    if (!orig_pthread_exit) {
        fprintf(stderr, "Unable to get original pthread_exit\n");
        exit(EXIT_FAILURE);
    }
    count--;
    orig_pthread_exit(retval);
}

static void check_locked_mutex(pthread_mutex_t *mutex) {
    int mutex_locked = pthread_mutex_trylock(mutex) == EBUSY;
    if (!mutex_locked) {
        pthread_mutex_unlock(mutex);
        fprintf(stderr, "Cannot wait with unlocked mutex!\n");
        exit(EXIT_FAILURE);
    }
}

int pthread_cond_wait(pthread_cond_t *restrict cond,
                      pthread_mutex_t *restrict mutex) {
    if (!orig_pthread_cond_wait) {
        fprintf(stderr, "Unable to get original pthread_cond_wait\n");
        exit(EXIT_FAILURE);
    }

    check_locked_mutex(mutex);

    if (rand() % 2) {
        return 0;
    } // TODO: assumes mutex and cv are correctly initialized
    return orig_pthread_cond_wait(cond, mutex);
}

int pthread_cond_timedwait(pthread_cond_t *restrict cond,
                           pthread_mutex_t *restrict mutex,
                           const struct timespec *restrict abstime) {
    if (!orig_pthread_cond_timedwait) {
        fprintf(stderr, "Unable to get original pthread_cond_timedwait\n");
        exit(EXIT_FAILURE);
    }

    check_locked_mutex(mutex);

    if (rand() % 2) {
        return 0;
    } // TODO: assumes mutex and cv are correctly initialized
    return orig_pthread_cond_timedwait(cond, mutex, abstime);
}

int pthread_cond_signal(pthread_cond_t *restrict cond) {
    if (!orig_pthread_cond_signal) {
        fprintf(stderr, "Unable to get original pthread_cond_signal\n");
        exit(EXIT_FAILURE);
    }

    return orig_pthread_cond_signal(cond);
}

int pthread_cond_broadcast(pthread_cond_t *restrict cond) {
    if (!orig_pthread_cond_broadcast) {
        fprintf(stderr, "Unable to get original pthread_cond_broadcast\n");
        exit(EXIT_FAILURE);
    }

    return orig_pthread_cond_broadcast(cond);
}

int pthread_cond_destroy(pthread_cond_t *restrict cond) {
    if (!orig_pthread_cond_destroy) {
        fprintf(stderr, "Unable to get original pthread_cond_destroy\n");
        exit(EXIT_FAILURE);
    }

    return orig_pthread_cond_destroy(cond);
}

void __attribute__((constructor)) my_init(void) {
    char *thread_limit = getenv("MAX_THREADS");
    if (thread_limit)
        MAX_THREADS = atoi(thread_limit);
    orig_pthread_create = dlsym(RTLD_NEXT, "pthread_create");
    orig_pthread_join = dlsym(RTLD_NEXT, "pthread_join");
    orig_pthread_cancel = dlsym(RTLD_NEXT, "pthread_cancel");
    orig_pthread_exit = dlsym(RTLD_NEXT, "pthread_exit");
    orig_pthread_cond_wait = dlsym(RTLD_NEXT, "pthread_cond_wait");
    orig_pthread_cond_timedwait = dlsym(RTLD_NEXT, "pthread_cond_timedwait");
    orig_pthread_cond_signal = dlsym(RTLD_NEXT, "pthread_cond_signal");
    orig_pthread_cond_broadcast = dlsym(RTLD_NEXT, "pthread_cond_broadcast");
    orig_pthread_cond_destroy = dlsym(RTLD_NEXT, "pthread_cond_destroy");
}
