/**
*  Lab
* CS 241 - Fall 2018
*/

#include "utils.h"
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <fcntl.h>

int **filedes_create(int num) {
    int **ret = (int **)calloc(num, sizeof(int *));
    for (int i = 0; i < num; i++) {
        ret[i] = (int *)calloc(2, sizeof(int));
    }

    return ret;
}

void filedes_destroy(int **fd, int num) {
    for (int i = 0; i < num; i++) {
        free(fd[i]);
    }
    free(fd);
}

int main(int argc, char **argv) {
    char *ptr;
    int numOfMappers = (int)strtol(argv[5], &ptr, 10);

    // Create an input pipe for each mapper.
    int **fd_splitter_to_mappers = filedes_create(numOfMappers);
    for (int i = 0; i < numOfMappers; i++) {
        pipe2(fd_splitter_to_mappers[i], O_CLOEXEC);
    }


    // Create one input pipe for the reducer.
    int fd_mappers_to_reducer[2];
    pipe2(fd_mappers_to_reducer, O_CLOEXEC);

    // Open the output file.
    int fd_output = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

    // Start a splitter process for each mapper.
    pid_t splitterChildren[numOfMappers];
    for (int i = 0; i < numOfMappers; i++) {
        pid_t child = fork();
        if (child < 0) {
            exit(1);
        } else if (child == 0) { // this is a child
            // close(fd_splitter_to_mappers[i][0]);
            char idx[10];
            sprintf(idx, "%d", i);
            dup2(fd_splitter_to_mappers[i][1], STDOUT_FILENO); // redirect the output of splitter to reducer...
            execlp("./splitter", "./splitter", argv[1], argv[5], idx, NULL);
            exit(1);
        } else {
            splitterChildren[i] = child;
        }
    }



    // Start all the mapper processes.
    pid_t mapperChildren[numOfMappers];

    for (int i = 0; i < numOfMappers; i++) {
        pid_t subchild = fork();
        if (subchild < 0) {
            exit(1);
        } else if (subchild == 0) { // this is a child
            close(fd_splitter_to_mappers[i][1]);
            dup2(fd_splitter_to_mappers[i][0], STDIN_FILENO);
            close(fd_splitter_to_mappers[i][0]);
            dup2(fd_mappers_to_reducer[1], STDOUT_FILENO);
            execlp(argv[3], argv[3], NULL);
            exit(1);
        } else {
            mapperChildren[i] = subchild;
        }
    }

    for (int i = 0; i < numOfMappers; i++) {
        close(fd_splitter_to_mappers[i][0]);
        close(fd_splitter_to_mappers[i][1]);
    }
    // close(fd_splitter_to_mappers[0]);
    // close(fd_splitter_to_mappers[1]);


    // Start the reducer process.
    pid_t reducerChild = fork();

    if (reducerChild < 0) {
        exit(1);
    } else if (reducerChild == 0) { // this is a child
        close(fd_mappers_to_reducer[1]);
        dup2(fd_output, STDOUT_FILENO);
        // for (int i = 0; i < numOfMappers; i++) {
        dup2(fd_mappers_to_reducer[0], STDIN_FILENO);
        close(fd_mappers_to_reducer[0]);

        // close(fd_reducer[1]);
        execlp(argv[4], argv[4], NULL);
        exit(1);
    } else {
        close(fd_mappers_to_reducer[0]);
        close(fd_mappers_to_reducer[1]);
    }


    // Wait for the reducer to finish.
    int reducerStatus, splittersStatus[numOfMappers], mappersStatus[numOfMappers];
    waitpid(reducerChild, &reducerStatus, 0);
    for (int i = 0; i < numOfMappers; i++) {
        waitpid(splitterChildren[i], &splittersStatus[i], 0);
        waitpid(mapperChildren[i], &mappersStatus[i], 0);
    }

    filedes_destroy(fd_splitter_to_mappers, numOfMappers);

    // Print nonzero subprocess exit codes.
    if (WEXITSTATUS(reducerStatus) != 0) {
        print_nonzero_exit_status(argv[4], WEXITSTATUS(reducerStatus));
    }
    for (int i = 0; i < numOfMappers; i++) {
        if (WEXITSTATUS(splittersStatus[i]) != 0) {
            print_nonzero_exit_status("splitter", WEXITSTATUS(splittersStatus[i]));
        }
        if (WEXITSTATUS(mappersStatus[i]) != 0) {
            print_nonzero_exit_status(argv[3], WEXITSTATUS(mappersStatus[i]));
        }
    }

    // Count the number of lines in the output file.
    close(fd_output);
    print_num_lines(argv[2]);

    return 0;
}
