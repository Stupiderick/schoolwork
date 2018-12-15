/**
* Password Cracker Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <stdlib.h>
#include <crypt.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "cracker1.h"
#include "format.h"
#include "utils.h"
#include "libs/queue.h"

typedef struct bundle {
    char usr[9];
    char hash[14];
    char knownPassword[9];
} bundle;

static int succeedCrackers = 0;
static int failedCrackers = 0;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
// static pthread_barrier_t b;
static queue *myqueue;

void *crackPassword(void *ptr) {
    bundle *curr;
    int thread_id = *(int *)ptr;
    while ((curr = (bundle *)queue_pull(myqueue)) != NULL) {
        struct crypt_data cdata;
        cdata.initialized = 0;

        v1_print_thread_start(thread_id, curr->usr);
        int succeed = 0;
        double startTime = getThreadCPUTime();

        // fprintf(stderr, "fp: %s, th: %s, ch: %s\n", fullPassword, tryHash, curr->hash);

        char password[9];
        size_t numOfDots = 0;
        // int count = 0;
        for (size_t i = 0; i < strlen(curr->knownPassword); i++) {
            // fprintf(stderr, "thread %d, char is: %c\n", thread_id, curr->knownPassword[i]);
            if (curr->knownPassword[i] == '.') {
                numOfDots++;
                password[i] = 'a';
            } else {
                password[i] = curr->knownPassword[i];
            }
        }
        password[strlen(curr->knownPassword)] = '\0';

        size_t numOfHashes = pow(26, numOfDots);

        for (size_t i = 1; i <= numOfHashes; i++) {
            const char *tryHash = crypt_r(password, "xx", &cdata);
            // fprintf(stderr, "fp: %s, th: %s, ch: %s\n", fullPassword, tryHash, curr->hash);
            // sleep(1);
            if (!strcmp(tryHash, curr->hash)) {
                pthread_mutex_lock(&m);
                succeedCrackers++;
                pthread_mutex_unlock(&m);
                succeed = 1;
                double usedTime = getThreadCPUTime() - startTime;
                v1_print_thread_result(thread_id, curr->usr, password, i, usedTime, 0);
                break;
            }
            incrementString(password);
            // fprintf(stderr, "it is: %s\n", remainedPassword);
            // sleep(1);
        }

        if (succeed == 0) {
            pthread_mutex_lock(&m);
            failedCrackers++;
            pthread_mutex_unlock(&m);
            double usedTime = getThreadCPUTime() - startTime;
            v1_print_thread_result(thread_id, curr->usr, NULL, numOfHashes - 1, usedTime, 1);
        }
        // free(remainedPassword);
        free(curr);
    }
    queue_push(myqueue, (bundle *)NULL);

    return NULL;
}

int start(size_t thread_count) {
    // TODO your code here, make sure to use thread_count!
    // Remember to ONLY crack passwords in other threads
    char *buf = NULL;
    size_t len = 0;
    ssize_t read;

    // pthread_barrier_init(&b, NULL, thread_count + 1);
    myqueue = queue_create(-1);

    while ((read = getline(&buf, &len, stdin)) != -1) {
        // buf[read - 1] = '\0';
        bundle *mybund = malloc(sizeof(bundle));
        // bundle mybund[sizeof(bundle)];
        sscanf(buf, "%s %s %s", mybund->usr, mybund->hash, mybund->knownPassword);
        //fprintf(stderr, "usr: %s, hash: %s, knownPassword: %s\n", mybund->usr, mybund->hash, mybund->knownPassword);
        queue_push(myqueue, mybund);
    }
    free(buf);
    queue_push(myqueue, (bundle *)NULL);

    int short_ids[thread_count];
    pthread_t ids[thread_count];

    for (size_t i = 0; i < thread_count; i++) {
        // ids[i] = i;
        short_ids[i] = i + 1;
        pthread_create(&ids[i], NULL, crackPassword, &short_ids[i]);
    }

    for (size_t i = 0; i < thread_count; i++) {
        pthread_join(ids[i], NULL);
    }

    v1_print_summary(succeedCrackers, failedCrackers);

    queue_destroy(myqueue);

    // pthread_barrier_destroy(&b);
    return 0; // DO NOT change the return code since AG uses it to check if your
              // program exited normally
}
