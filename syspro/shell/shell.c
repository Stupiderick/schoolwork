/**
* Shell Lab
* CS 241 - Fall 2018
*/

#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>
#include "format.h"
#include "shell.h"
#include "vector.h"
#include "sstring.h"

typedef struct process {
    char *command;
    char *status;
    pid_t pid;
} process;

static pid_t childId;
static vector *vpro;

void *p_copy_constructor(void *elem) {
    if (!elem) {
        return NULL;
    }
    process *e = (process *)elem;
    process *copy = malloc(sizeof(process));
    copy->command = malloc(sizeof(e->command) + 1);
    copy->command = e->command;
    copy->status = e->status;
    copy->pid = e->pid;
    return (void *)copy;
}

void p_destructor(void *elem) {
    process *e = (process *)elem;
    free(e->command);
    free(e);
}

void *p_default_constructor(void) {
    process *ret = malloc(sizeof(process));
    ret->command = calloc(1, sizeof(char));

    return (void *)ret;
}

void SIGCHLD_handler(int signal) {
    int status;

    for (size_t i = 0; i < vector_size(vpro); i++) {
        pid_t p = ((process *)vector_get(vpro, i))->pid;
        if (waitpid(p, &status, WNOHANG) > 0) {
            if (p != -1 && WIFEXITED(status)) {
                vector_erase(vpro, i);
            }
        }
    }
}

void SIGINT_handler(int signal) {
    kill(childId, SIGINT);
}

int shell(int argc, char *argv[]) {
    // TODO: This is the entry point for your shell.
    signal(SIGCHLD, SIGCHLD_handler);
    signal(SIGINT, SIGINT_handler);
    //signal(SIGTERM, SIGTERM_handler);
    int opt;
    int flagh = 0;
    int flags = 0;
    FILE *hist;
    FILE *script;
    char *histloc;
    int real_exit = 0;

    // Check the input...
    while ((opt = getopt(argc, argv, "h:f:")) != -1) {
        switch(opt) {
        case 'h':
            flagh = 1;
            hist = fopen(optarg, "a+");
            histloc = optarg;
            if (hist == NULL) {
                print_history_file_error();
                exit(1);
            }
            break;
        case 'f':
            flags = 1;
            script = fopen(optarg, "r");
            if (script == NULL) {
                if (flagh == 1) {
                    fclose(hist);
                }
                print_script_file_error();
                exit(1);
            }
            break;
        }
    }

    // check argc...
    if (flagh == 0 && flags == 0) {
        if (argc != 1) {
            print_usage();
            exit(1);
        }
    } else if ((flagh == 1 && flags == 0) || (flagh == 0 && flags == 1)) {
        if (argc != 3 && flagh == 1) {
            fclose(hist);
            print_usage();
            exit(1);
        } else if (argc != 3 && flags == 1) {
            fclose(script);
            print_usage();
            exit(1);
        }
    } else {
        if (argc != 5) {
            fclose(hist);
            fclose(script);
            print_usage();
            exit(1);
        }
    }

    vpro = vector_create(p_copy_constructor, p_destructor, p_default_constructor);

    // load hist file or create a temp one...
    vector *vhist = vector_create(string_copy_constructor, string_destructor, string_default_constructor);
    if (flagh != 0) {
        char *buf = NULL;
        size_t len = 0;
        int nread;

        while ((nread = getline(&buf, &len, hist)) != -1) {
            char *pos3;
            if ((pos3 = strchr(buf, '\n')) != NULL) {
                *pos3 = '\0';
            }
            vector_push_back(vhist, buf);
        }
        free(buf);
        fclose(hist);
        hist = fopen(histloc, "w+");
    }
    //printf("%s\n", vector_get(vhist, 0));

    int curr_pid = getpid();
    char * curr_path = get_current_dir_name();
    char str[300];

    // if cmd file is passed in, script will be the file, otherwise will be stdin.
    if (flags == 0) {
        script = stdin;
    }

    int machine_input = 0;

    // the great WHILE LOOPIE...
    print_prompt(curr_path, curr_pid);
    int pp = 1;

    while (1) {
        if (machine_input == 0) {
            if (fgets(str, 100, script) == NULL) {
                break;
            }
        } else {
            machine_input = 0;
        }

        char *pos;
        if ((pos=strchr(str, '\n')) != NULL) {
            *pos = '\0';
        }

        if (flags != 0) {
            print_command(str);
        }

        // handle multiple...
        int op_mode = 0;
        int second_exe = 0;
        int exe_succeeded = 1;

        vector *opcmd = vector_create(string_copy_constructor, string_destructor, string_default_constructor);

        sstring *aa = cstr_to_sstring(str);
        sstring_substitute(aa, 0, "&&", "__");
        char *bb = sstring_to_cstr(aa);

        char *checkerA = strdup(bb);
        char *checker1 = checkerA;
        //char *tc = checker;
        char *found1;
        int ampamp = 0;
        while((found1 = strsep(&checker1, "__")) != NULL) {
            ampamp++;
            //char *dup = found1;
            vector_push_back(opcmd, found1);
            //printf("what %s\n", found1);
        }

        if (ampamp != 1) {
            op_mode = 1;
            vector_push_back(vhist, str);
        } else {
            vector_pop_back(opcmd);
        }

        //strcpy(str, bb);
        free(bb);
        sstring_destroy(aa);

        //printf("yessss: %s\n", vector_get(opcmd, 1));

        char *checkerB = strdup(str);
        char *checker2 = checkerB;
        //char *tc = checker;
        char *found2;
        int dashdash = 0;

        while((found2 = strsep(&checker2, "||")) != NULL) {
            dashdash++;
            vector_push_back(opcmd, found2);
        }

        if (dashdash != 1) {
           op_mode = 2;
           vector_push_back(vhist, str);
        } else {
            vector_pop_back(opcmd);
        }

        char *checkerC = strdup(str);
        char *checker3 = checkerC;
        char *found3;
        int semi = 0;

        while((found3 = strsep(&checker3, ";")) != NULL) {
            semi++;
            vector_push_back(opcmd, found3);
        }


        if (semi != 1) {
            op_mode = 3;
            vector_push_back(vhist, str);
        } else {
            vector_pop_back(opcmd);
        }

        if (vector_size(opcmd) == 0) {
            vector_push_back(opcmd, str);
            //
        }

        // remove empty string...
        void **tra2 = vector_front(opcmd);
        int cc = 0;
        while (*tra2 != NULL) {
            if (strlen(vector_get(opcmd, cc)) == 0) {
                vector_erase(opcmd, cc);
            } else {
                cc++;
                tra2++;
            }
        }

        free(checkerA);
        free(checkerB);
        free(checkerC);

        //printf("sizzzze: %s\n", vector_get(opcmd, 0));

        for (size_t rr = 0; rr < vector_size(opcmd); rr++) {
            if (second_exe == 0) {
                second_exe = 1;
            } else {
                if (op_mode == 1) {
                    if (exe_succeeded == 0) {
                        //printf("hi\n");
                        break;
                    }
                } else if (op_mode == 2) {
                    if (exe_succeeded == 1) {
                        break;
                    }
                }
            }
            //printf("yessss: %s\n", vector_get(opcmd, rr));
            strcpy(str, vector_get(opcmd, rr));
            char *pos2;
            int flagamp = 0;
            //printf("ttt %s %d\n", str, flagamp);
            if ((pos2 = strrchr(str, '&')) != NULL) {
                vector_push_back(vhist, str);
                flagamp = 1;
                *pos2 = '\0';
            }

            sstring *sscmd = cstr_to_sstring(str);
            vector *vcmd = (vector *)sstring_split(sscmd, ' ');

            // remove empty string...
            void **tra = vector_front(vcmd);
            int ccc = 0;
            while (*tra != NULL) {
                if (strlen(vector_get(vcmd, ccc)) == 0) {
                    vector_erase(vcmd, ccc);
                } else {
                    ccc++;
                    tra++;
                }
            }

            // CASE: 'cd'
            if (!strcmp(vector_get(vcmd, 0), "cd")) {
                if (vector_size(vcmd) < 2) {
                    print_no_directory("");
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    continue;
                }
                char *new_path;
                if (((char *)vector_get(vcmd, 1))[0] != '/') {
                    sstring *temp1 = cstr_to_sstring(curr_path);
                    sstring *temp2 = cstr_to_sstring("/");
                    sstring *temp3 = cstr_to_sstring((char *)vector_get(vcmd, 1));
                    sstring_append(temp1, temp2);
                    sstring_append(temp1, temp3);
                    sstring_destroy(temp2);
                    sstring_destroy(temp3);
                    new_path = sstring_to_cstr(temp1);
                    sstring_destroy(temp1);
                } else {
                    sstring *te = cstr_to_sstring(vector_get(vcmd, 1));
                    new_path = sstring_to_cstr(te);
                    sstring_destroy(te);
                }
                int check_cd = chdir((const char *)new_path);
                // if no dir found...
                if (check_cd == -1) {
                    print_no_directory(new_path);
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    free(new_path);
                    //print_prompt(curr_path, curr_pid);
                    continue;
                } else {
                    free(curr_path);
                    //curr_path = (char *)malloc(strlen(new_path) + 1);
                    curr_path = get_current_dir_name();
                    free(new_path);
                    if (op_mode == 0) {
                        vector_push_back(vhist, str);
                    }
                }
                // CASE: !history
            } else if (!strcmp(vector_get(vcmd, 0), "!history")) {
                if (vector_size(vcmd) == 1) {
                    for (size_t i = 0; i < vector_size(vhist); i++) {
                        print_history_line(i, vector_get(vhist, i));
                    }
                } else {
                    print_invalid_command(str);
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    //print_prompt(curr_path, curr_pid);
                    continue;
                }
                // CASE: #<n>
            } else if (((char *)vector_get(vcmd, 0))[0] == '#') {
                if (vector_size(vcmd) == 1) {
                    char *temp = vector_get(vcmd, 0);
                    temp++;
                    char *check;
                    int n = (int)strtol(temp, &check, 10);
                    if (strlen(check) != 0) {
                        print_invalid_command(str);
                        sstring_destroy(sscmd);
                        vector_destroy(vcmd);
                        //print_prompt(curr_path, curr_pid);
                        continue;
                    } else {
                        if (n >= (int)vector_size(vhist)) {
                            print_invalid_index();
                            sstring_destroy(sscmd);
                            vector_destroy(vcmd);
                            //print_prompt(curr_path, curr_pid);
                            continue;
                        } else {
                            sstring_destroy(sscmd);
                            vector_destroy(vcmd);
                            strcpy(str, vector_get(vhist, n));
                            machine_input = 1;
                            pp = 0;
                            print_command(str);
                            continue;
                        }
                    }
                }
                // CASE: !<prefix>
            } else if (((char *)vector_get(vcmd, 0))[0] == '!') {
                if (vector_empty(vhist)) {
                    print_no_history_match();
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    continue;
                }
                int no_need_to_loop = -1;
                int flagfound = 0;
                if (!strcmp((char *)vector_get(vcmd, 0), "!")) {
                    no_need_to_loop = (int)vector_size(vhist) - 1;
                    flagfound = 1;
                } else {
                    char *ttemp;
                    for (size_t qq = 0; qq < vector_size(vhist); qq++) {
                        ttemp = str;
                        ttemp++;
                        flagfound = 1;
                        char *vline = vector_get(vhist, qq);
                        while (*ttemp != '\0' && *vline != '\0') {
                            if (*ttemp != *vline) {
                                flagfound = 0;
                                break;
                            }
                            ttemp++;
                            vline++;
                        }
                        if (flagfound == 1) {
                            no_need_to_loop = (int)qq;
                            break;
                        }
                    }
                }

                // need loop..
                if (no_need_to_loop != -1) {
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    strcpy(str, vector_get(vhist, no_need_to_loop));
                    machine_input = 1;
                    print_command(str);
                    pp = 0;
                    //printf("whbcshvbjab\n");
                    continue;
                }
                if (flagfound == 0) {
                    print_no_history_match();
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    //print_prompt(curr_path, curr_pid);
                    continue;
                }
            } else if (!strcmp(vector_get(vcmd, 0), "ps")) {
                for (size_t i = 0; i < vector_size(vpro); i++) {
                    process *cp = vector_get(vpro, i);
                    print_process_info(cp->status, cp->pid, cp->command);
                }
                // CASE: kill
            } else if (!strcmp(vector_get(vcmd, 0), "kill")) {
                if (vector_size(vcmd) < 2) {
                    print_invalid_command(str);
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    //print_prompt(curr_path, curr_pid);
                    continue;
                } else {
                    char *temp;
                    int n = (int)strtol(vector_get(vcmd, 1), &temp, 10);
                    if (strlen(temp) != 0) {
                        print_invalid_command(str);
                        sstring_destroy(sscmd);
                        vector_destroy(vcmd);
                        //print_prompt(curr_path, curr_pid);
                        continue;
                    } else {
                        for (size_t ll = 0; ll < vector_size(vpro); ll++) {
                            if (((process *)vector_get(vpro, ll))->pid == n) {
                                if (((process *)vector_get(vpro, ll))->status != STATUS_RUNNING) {
                                    int retc = kill((pid_t)n, SIGCONT);
                                    if (retc == -1) {
                                        print_no_process_found((pid_t)n);
                                        sstring_destroy(sscmd);
                                        vector_destroy(vcmd);
                                        //print_prompt(curr_path, curr_pid);
                                        continue;
                                    } else {
                                        ((process *)vector_get(vpro, ll))->status = STATUS_RUNNING;
                                    }
                                }
                                break;
                            }
                        }

                        int retk = kill((pid_t)n, SIGTERM);
                        wait(NULL);
                        if (retk == -1) {
                            print_no_process_found((pid_t)n);
                            sstring_destroy(sscmd);
                            vector_destroy(vcmd);
                            //print_prompt(curr_path, curr_pid);
                            continue;
                        } else {
                            print_killed_process((pid_t)n, str);
                            if (op_mode == 0) {
                                vector_push_back(vhist, str);
                            }

                            for (size_t ll = 0; ll < vector_size(vpro); ll++) {

                                if (((process *)vector_get(vpro, ll))->pid == n) {
                                    //printf("hello\n");
                                    vector_erase(vpro, ll);
                                    break;
                                }
                            }
                            //printf("vp %s\n", ((process *)vector_get(vpro, 0))->command);
                        }
                    }
                }
                // CASE: stop
            } else if (!strcmp(vector_get(vcmd, 0), "stop")) {
                if (vector_size(vcmd) < 2) {
                    print_invalid_command(str);
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    //print_prompt(curr_path, curr_pid);
                    continue;
                } else {
                    char *temp;
                    int n = (int)strtol(vector_get(vcmd, 1), &temp, 10);
                    if (strlen(temp) != 0) {
                        print_invalid_command(str);
                        sstring_destroy(sscmd);
                        vector_destroy(vcmd);
                        //print_prompt(curr_path, curr_pid);
                        continue;
                    } else {
                        int retk = kill((pid_t)n, SIGTSTP);
                        if (retk == -1) {
                            print_no_process_found((pid_t)n);
                            sstring_destroy(sscmd);
                            vector_destroy(vcmd);
                            //print_prompt(curr_path, curr_pid);
                            continue;
                        } else {
                            print_stopped_process((pid_t)n, str);
                            if (op_mode == 0) {
                                vector_push_back(vhist, str);
                            }

                            for (size_t ll = 0; ll < vector_size(vpro); ll++) {
                                if (((process *)vector_get(vpro, ll))->pid == n) {
                                    ((process *)vector_get(vpro, ll))->status = STATUS_STOPPED;
                                    break;
                                }
                            }
                        }
                    }
                }
                // CASE: cont
            } else if (!strcmp(vector_get(vcmd, 0), "cont")) {
                if (vector_size(vcmd) < 2) {
                    print_invalid_command(str);
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    //print_prompt(curr_path, curr_pid);
                    continue;
                } else {
                    char *temp;
                    int n = (int)strtol(vector_get(vcmd, 1), &temp, 10);
                    if (strlen(temp) != 0) {
                        print_invalid_command(str);
                        sstring_destroy(sscmd);
                        vector_destroy(vcmd);
                        //print_prompt(curr_path, curr_pid);
                        continue;
                    } else {
                        int retk = kill((pid_t)n, SIGCONT);
                        if (retk == -1) {
                            print_no_process_found((pid_t)n);
                            sstring_destroy(sscmd);
                            vector_destroy(vcmd);
                            //print_prompt(curr_path, curr_pid);
                            continue;
                        } else {
                            //print_stopped_process((pid_t)n, str);
                            if (op_mode == 0) {
                                vector_push_back(vhist, str);
                            }

                            for (size_t ll = 0; ll < vector_size(vpro); ll++) {
                                if (((process *)vector_get(vpro, ll))->pid == n) {
                                    ((process *)vector_get(vpro, ll))->status = STATUS_RUNNING;
                                    break;
                                }
                            }
                        }
                    }
                }
                // CASE: exit
            } else if (!strcmp(vector_get(vcmd, 0), "exit")) {
                sstring_destroy(sscmd);
                vector_destroy(vcmd);
                real_exit = 1;
                break;
            } else {
                childId = fork();
                if (childId == -1) {
                    print_fork_failed();
                    sstring_destroy(sscmd);
                    vector_destroy(vcmd);
                    if (flagh == 1) {
                        fclose(hist);
                    }
                    if (flags == 1) {
                        fclose(script);
                    }
                    //printf("hi\n");
                    exit(1);
                } else if (childId == 0) {
                    size_t vs = vector_size(vcmd);
                    char **arr = (char **)malloc(sizeof(char *) * (vs + 1));
                    for (size_t i = 0; i < vs; i++) {
                        char *t = vector_get(vcmd, i);
                        arr[i] = (char *)malloc(strlen(t) + 1);
                        strcpy(arr[i], t);
                    }
                    arr[vs] = NULL;

                    // In child...
                    execvp(vector_get(vcmd, 0), arr);
                    print_exec_failed(str);


                    for (size_t i = 0; i < vs; i++) {
                        free(arr[i]);
                    }
                    free(arr);
                    sstring_destroy(sscmd);
                    vector_destroy(opcmd);
                    free(curr_path);
                    vector_destroy(vpro);
                    vector_destroy(vhist);
                    vector_destroy(vcmd);
                    if (flagh == 1) {
                        fclose(hist);
                    }
                    if (flags == 1) {
                        fclose(script);
                    }
                    exit(1);
                } else {
                    int stat = 0;
                    print_command_executed(childId);

                    // run foreground
                    if (flagamp != 1) {
                        if (op_mode == 0) {
                            vector_push_back(vhist, str);
                        }

                        pid_t res = waitpid(childId, &stat, 0);
                        if (res == -1) {
                            print_wait_failed();
                            sstring_destroy(sscmd);
                            vector_destroy(vcmd);
                            continue;
                        }
                    } else {
                        // run background
                        if (setpgid(childId, childId) == -1) {
                            print_setpgid_failed();
                            vector_destroy(vcmd);
                            sstring_destroy(sscmd);
                            vector_destroy(opcmd);
                            free(curr_path);
                            vector_destroy(vpro);
                            vector_destroy(vhist);
                            if (flagh == 1) {
                                fclose(hist);
                            }
                            if (flags == 1) {
                                fclose(script);
                            }
                            exit(1);
                        }

                        // TODO: create processes struct...
                        process *myp = malloc(sizeof(process));
                        myp->command = strdup(str);
                        myp->status = STATUS_RUNNING;
                        myp->pid = childId;
                        vector_push_back(vpro, myp);
                        //printf("eee %s\n", ((process *)vector_get(vpro, 0))->command);
                        free(myp);
                    }

                    if (WIFEXITED(stat)) {
                        int es = WEXITSTATUS(stat);
                        if (es != 1) {
                            exe_succeeded = 1;
                        } else {
                            exe_succeeded = 0;
                        }
                    }
                }
            }
            vector_destroy(vcmd);
            sstring_destroy(sscmd);
        }
        if (real_exit) {
            vector_destroy(opcmd);
            break;
        }
        vector_destroy(opcmd);

        if (pp == 0) {
            pp = 1;
        } else {
            print_prompt(curr_path, curr_pid);
        }
    }

    // wrote to hist file
    if (flagh == 1) {
        for (size_t ee = 0; ee < vector_size(vhist); ee++) {
            fprintf(hist, "%s\n", vector_get(vhist, ee));
        }
        fclose(hist);
    }
    // close script file..
    if (flags == 1) {
        fclose(script);
    }

    size_t vss = vector_size(vpro);
    printf("%zu\n", vss);
    for (size_t ww = 0; ww < vss; ww++) {
        pid_t cpid = ((process *)vector_get(vpro, 0))->pid;
        char * cstatus = ((process *)vector_get(vpro, 0))->status;
        if (!strcmp(cstatus, STATUS_RUNNING)) {
            int retc = kill(cpid, SIGCONT);
            if (retc == -1) {
                print_no_process_found(cpid);
                continue;
            } else {
                ((process *)vector_get(vpro, 0))->status = STATUS_RUNNING;
            }
        }

        int retk = kill(cpid, SIGTERM);
        wait(NULL);
        vector_erase(vpro, 0);
        if (retk == -1) {
            print_no_process_found(cpid);
            continue;
        }
    }

    vector_destroy(vhist);
    free((void *)curr_path);
    vector_destroy(vpro);

    return 0;
}
