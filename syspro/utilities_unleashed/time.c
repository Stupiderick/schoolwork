/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/

#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <wait.h>
#include <stdlib.h>
#include "format.h"
#include <string.h>

#define BILLION 1000000000.00

int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_time_usage();
    }

    char *realArgv[argc];
    for (int i = 1; i < argc; i++) {
        realArgv[i - 1] = malloc(strlen(argv[i]) + 1);
        strcpy(realArgv[i - 1], argv[i]);
    }
    realArgv[argc - 1] = NULL;


    struct timespec start, stop;
    int status = 0;

    pid_t childId = fork();
    if (childId == -1) {
        // when fork is getting wrong...
        print_fork_failed();
    } else if (childId == 0) {
        // In child...
        execvp(argv[1], realArgv);
        print_exec_failed();
    } else {
        // In parent...
        clock_gettime(CLOCK_MONOTONIC, &start);
        wait(&status);
        clock_gettime(CLOCK_MONOTONIC, &stop);
    }

    if (WIFEXITED(status)) {
        int es = WEXITSTATUS(status);
        if (es != 1) {
            double timeDiff = (BILLION * (stop.tv_sec - start.tv_sec) + stop.tv_nsec - start.tv_nsec) / BILLION;
            display_results(argv, timeDiff);
        }
    }
    
    for (int i = 0; i < argc - 1; i++) {
        free(realArgv[i]);
    }

    return 0;
}
