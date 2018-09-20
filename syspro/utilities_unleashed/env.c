/**
* Utilities Unleashed Lab
* CS 241 - Fall 2018
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wait.h>
#include <unistd.h>
#include "format.h"

// destroy keys and vals...
void destroyer(char **keys, char ***vals) {
    char **temp = keys;
    while (*temp != NULL) {
        free(*temp);
        *temp = NULL;
        temp++;
    }
    free(keys);
    keys = NULL;
    temp = NULL;

    char ***tempv1 = vals;
    while (*tempv1 != NULL) {
        char **tempv2 = *tempv1;
        while (*tempv2 != NULL) {
            free(*tempv2);
            tempv2++;
        }
        free(*tempv1);
        tempv1++;
    }
    free(vals);
    vals = NULL;
}


int main(int argc, char *argv[]) {
    if (argc == 1) {
        print_env_usage();
    }
    int numOfVal = 1;
    // if we are still parsing keys: 1; after entering cmd: 0
    int keyFlag = 1;
    int start = 0;

    // With "!", if strs are the same, enter into the if...
    if (!strcmp(argv[1], "-n")) {
        if (argv[2] == NULL) {
            print_env_usage();
        }
        if (argv[3] == NULL) {
            print_env_usage();
        }

        char *ptr;
        // get N.
        numOfVal = (int)strtol(argv[2], &ptr, 10);

        if (strlen(ptr) != 0) {
            print_env_usage();
        }
        start = 3;
    } else {
        start = 1;
    }

    char **keys = (char **)malloc(sizeof(char *));
    char ***vals = (char ***)malloc(sizeof(char **));
    char *realArgv[argc];

    int count = 0;
    int countcmd = 0;
    int countk = 0;

    for (int i = start; i < argc; i++) {
        if (!strcmp(argv[i], "--")) {
            keyFlag = 0;
            continue;
        }

        if (keyFlag == 1) {
            // check if there are more than one '='
            int countequal = 0;
            for (int p = 0; p < (int)strlen(argv[i]); p++) {
                if (argv[i][p] == '=') {
                    countequal++;
                }
            }
            if (countequal != 1) {
                keys[countk] = NULL;
                vals[countk] = NULL;
                destroyer(keys, vals);
                print_env_usage();
            }

            // Parse "=" first...
            char *tempk = strdup(argv[i]);
            char *tokk = tempk;
            char *endk = tempk;

            int numOfStr = 0;
            while (tokk != NULL) {
                // parse key...
                if (numOfStr == 0) {
                    strsep(&endk, "=");
                    // check if the KEY only has letters, digits, or underscore...
                    for (int e = 0; e < (int)strlen(tokk); e++) {
                        if (!isdigit(tokk[e]) && !isalpha(tokk[e]) && tokk[e] != '_') {
                            keys[countk] = NULL;
                            vals[countk] = NULL;
                            destroyer(keys, vals);
                            free(tempk);
                            print_env_usage();
                        }
                    }
                    keys[count] = malloc(strlen(tokk) + 1);
                    strcpy(keys[count], tokk);
                    //printf("%s\n", keys[count]);
                    keys = (char **)realloc(keys, sizeof(char *) * (count + 2));
                    tokk = endk;
                // parse vals...
                } else {
                    strsep(&endk, "=");

                    char *tempv = strdup(tokk);
                    char *tokv = tempv;
                    char *endv = tempv;
                    vals = (char ***)realloc(vals, sizeof(char **) * (count + 2));
                    vals[count] = (char **)malloc(sizeof(char *));
                    int smallnum = 0;
                    while (tokv != NULL) {
                        strsep(&endv, ",");
                        for (int e = 0; e < (int)strlen(tokv); e++) {
                            if (e == 0) {
                                if (!isdigit(tokv[e]) && !isalpha(tokv[e]) && tokv[e] != '_' && tokv[e] != '%') {
                                    keys[countk + 1] = NULL;
                                    vals[countk][smallnum] = NULL;
                                    vals[countk + 1] = NULL;
                                    destroyer(keys, vals);
                                    free(tempv);
                                    free(tempk);
                                    print_env_usage();
                                }
                            } else {
                                if (!isdigit(tokv[e]) && !isalpha(tokv[e]) && tokv[e] != '_') {
                                    keys[countk + 1] = NULL;
                                    vals[countk][smallnum] = NULL;
                                    vals[countk + 1] = NULL;
                                    destroyer(keys, vals);
                                    free(tempv);
                                    free(tempk);
                                    print_env_usage();
                                }
                            }
                        }
                        vals[count][smallnum] = malloc(strlen(tokv) + 1);
                        strcpy(vals[count][smallnum], tokv);
                        //printf(" %s\n", vals[count][smallnum]);
                        vals[count] = (char **)realloc(vals[count], sizeof(char *) * (smallnum + 2));
                        tokv = endv;
                        smallnum++;
                    }
                    vals[count][smallnum] = NULL;
                    tokk = endk;
                    free(tempv);
                    // if the number of values is neither 1 nor N...
                    if (smallnum != 1 && smallnum != numOfVal) {
                        keys[countk + 1] = NULL;
                        vals[countk + 1] = NULL;
                        destroyer(keys, vals);
                        free(tempk);
                        print_env_usage();
                    }
                    if (smallnum == 1) {
                        vals[count] = (char **)realloc(vals[count], sizeof(char *) * (numOfVal + 1));
                        for (int v = 1; v < numOfVal; v++) {
                            vals[count][v] = malloc(strlen(vals[count][0]) + 1);
                            strcpy(vals[count][v], vals[count][0]);
                        }
                        vals[count][numOfVal] = NULL;
                    }
                }
                numOfStr++;
            }
            free(tempk);
            countk++;
        // cmd line
        } else {
            realArgv[countcmd] = malloc(strlen(argv[i]) + 1);
            strcpy(realArgv[countcmd], argv[i]);
            countcmd++;
        }
        count++;
    }

    keys[countk] = NULL;
    vals[countk] = NULL;
    realArgv[countcmd] = NULL;

    // Start exec part...
    int status = 0;
    for (int j = 0; j < numOfVal; j++) {
        pid_t child = fork();
        if (child == -1) {
            print_fork_failed();
        }
        if (child == 0) {
            for (int k = 0; k < countk; k++) {
                //printf("-------%s\n", vals[k][j]);
                int ret = 0;
                if (vals[k][j][0] == '%') {
                    //printf("yyyyy\n");
                    char *hey = vals[k][j] + 1;
                    char *yes = getenv(hey);
                    if (yes == NULL) {
                       /* destroyer(keys, vals);
                        for (int u = 0; u < countcmd; u++) {
                           free(realArgv[u]);
                        }
                        print_environment_change_failed();*/
                        yes = "";
                    }
                    ret = setenv(keys[k], yes, 1);
                } else {
           //         printf("-------%s\n", vals[k][j]);
                    ret = setenv(keys[k], vals[k][j], 1);
                }
                //printf("ggggggg\n");
                if (ret == -1) {
                    destroyer(keys, vals);
                    for (int u = 0; u < countcmd; u++) {
                        free(realArgv[u]);
                    }
                    print_environment_change_failed();
                }
            }
            execvp(realArgv[0], realArgv);
            destroyer(keys, vals);
            for (int u = 0; u < countcmd; u++) {
                free(realArgv[u]);
            }
            print_exec_failed();
        } else {
            wait(&status);
        }
    }

    destroyer(keys, vals);
    for (int u = 0; u < countcmd; u++) {
        free(realArgv[u]);
    }

    return 0;
}
