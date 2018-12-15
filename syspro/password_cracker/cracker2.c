/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>
#include <unistd.h>
#include <stdatomic.h>

#include "cracker2.h"
#include "format.h"
#include "utils.h"

typedef struct bundle {
    char usr[9];
    char hash[14];
    char knownPassword[9];
    size_t thread_count;
} bundle;

static bundle *mybund;
volatile atomic_int thread_status;
static char foundPassword[9];
static int hashCount;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t b1;
pthread_barrier_t b2;
pthread_barrier_t b3;

void *crackPassword(void *ptr) {
    int thread_id = *(int *)ptr;

    while (1) {
        pthread_barrier_wait(&b1);
        if (mybund == NULL) {
            break;
        }

        struct crypt_data cdata;
        cdata.initialized = 0;

        long startIdx;
        long tryCount;
        // long count = 0;
        char password[9];
        size_t numOfDots = 0;
        // pthread_mutex_lock(&m);
        char *knownPassword = strdup(mybund->knownPassword);
        char *user = strdup(mybund->usr);
        char *hash = strdup(mybund->hash);
        int tc = mybund->thread_count;
        // pthread_mutex_unlock(&m);

        for (size_t i = 0; i < strlen(knownPassword); i++) {
            // fprintf(stderr, "thread %d, char is: %c\n", thread_id, curr->knownPassword[i]);
            if (knownPassword[i] == '.') {
                numOfDots++;
                password[i] = '\0';
            } else {
                password[i] = knownPassword[i];
            }
        }

        char remainedPassword[numOfDots + 1];
        for (size_t i = 0; i < numOfDots; i++) {
            remainedPassword[i] = 'a';
        }
        remainedPassword[numOfDots] = '\0';
        // fprintf(stderr, "original password: %s\n", password);
        getSubrange(numOfDots, tc, thread_id, &startIdx, &tryCount);

        setStringPosition(remainedPassword, startIdx);
        strcat(password, remainedPassword);

        // fprintf(stderr, "then password: %s\n", password);
        v2_print_thread_start(thread_id, user, startIdx, password);

        // double startTime = getThreadCPUTime();
        pthread_barrier_wait(&b3);
        int breakout = 0;
        for (size_t i = startIdx; i < (size_t)(startIdx + tryCount); i++) {
            // pthread_mutex_lock(&m);
            if (thread_status == 0) {
                pthread_mutex_lock(&m);
                v2_print_thread_result(thread_id, i - startIdx + 1, 1);
                hashCount += i - startIdx + 1;
                pthread_mutex_unlock(&m);
                breakout = 1;

                break;
            }
            // pthread_mutex_unlock(&m);

            // fprintf(stderr, "password is: %s\n", password);
            // sleep(2);

            const char *tryHash = crypt_r(password, "xx", &cdata);
            // fprintf(stderr, "fp: %s, th: %s, ch: %s\n", fullPassword, tryHash, curr->hash);
            // sleep(1);
            // count++;
            if (!strcmp(tryHash, hash)) {
                // double usedTime = getThreadCPUTime() - startTime;
                pthread_mutex_lock(&m);
                v2_print_thread_result(thread_id, i - startIdx + 1, 0);
                hashCount += i - startIdx + 1;
                // pthread_mutex_lock(&m);
                thread_status = 0;
                strcpy(foundPassword, password);
                pthread_mutex_unlock(&m);
                // pthread_barrier_wait(&b2);
                breakout = 1;
                break;
            }
            incrementString(password);
        }

        if (breakout == 0) {
            pthread_mutex_lock(&m);
            v2_print_thread_result(thread_id, tryCount, 2);
            hashCount += tryCount;
            pthread_mutex_unlock(&m);
        }

        free(user);
        free(hash);
        free(knownPassword);

        // fprintf(stderr, "before wait b2: %d; the end: %d\n", thread_id, theEnd);
        pthread_barrier_wait(&b2);
    }

    // fprintf(stderr, "to the end: %d\n", thread_id);

    return NULL;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads

    int short_ids[thread_count];
    pthread_t ids[thread_count];
    // theEnd = 0;
    pthread_barrier_init(&b1, NULL, thread_count + 1);
    pthread_barrier_init(&b2, NULL, thread_count + 1);
    pthread_barrier_init(&b3, NULL, thread_count);

    for (size_t i = 0; i < thread_count; i++) {
        short_ids[i] = i + 1;
        pthread_create(&ids[i], NULL, crackPassword, &short_ids[i]);
    }

    char *buf = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&buf, &len, stdin)) != -1) {
        hashCount = 0;
        buf[read - 1] = '\0';
        pthread_mutex_lock(&m);
        mybund = malloc(sizeof(bundle));
        thread_status = 1;
        sscanf(buf, "%s %s %s", mybund->usr, mybund->hash, mybund->knownPassword);
        // fprintf(stderr, "user: %s\n", mybund->usr);
        mybund->thread_count = thread_count;
        v2_print_start_user(mybund->usr);
        pthread_mutex_unlock(&m);
        double start_time = getTime();
        double start_cpu_time = getCPUTime();
        pthread_barrier_wait(&b1);

        pthread_barrier_wait(&b2);
        double elapsed = getTime() - start_time;
        double total_cpu_time = getCPUTime() - start_cpu_time;
        v2_print_summary(mybund->usr, foundPassword, hashCount, elapsed, total_cpu_time, thread_status);
        // pthread_barrier_init(&b2, NULL, thread_count + 1);
        pthread_mutex_lock(&m);
        free(mybund);
        mybund = NULL;
        pthread_mutex_unlock(&m);
    }

    free(buf);
    pthread_barrier_wait(&b1);

    for (size_t i = 0; i < thread_count; i++) {
        pthread_join(ids[i], NULL);
    }

    pthread_barrier_destroy(&b1);
    pthread_barrier_destroy(&b2);
    pthread_barrier_destroy(&b3);

    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
