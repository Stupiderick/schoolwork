/**
* Networking Lab
* CS 241 - Fall 2018
*/

#include "common.h"
#define SINGLE_BUF_SIZE 23000

ssize_t get_from_server(int socket, char *filename) {
    char *status = malloc(6);
    char *tracker = status;
    for (size_t i = 0; i < 6; i++) {
        int ret = read(socket, status + i, 1);
        if (ret == -1 && errno != EINTR) {
            return -1;
        }
        if (*tracker == '\n') {
            *tracker = '\0';
            break;
        }
        tracker++;
    }

    fprintf(stdout, "STATUS_%s\n", status);
    if (!strcmp(status, "ERROR")) {
        free(status);
        size_t erridx = 0;
        size_t errsize = 50;
        char *errmesg = malloc(errsize);
        char *errtr = errmesg;
        while (1) {
            if (erridx == errsize) {
                errsize *= 2;
                errmesg = realloc(errmesg, errsize);
            }
            int ret = read(socket, errmesg + erridx, 1);
            if (ret == -1 && errno != EINTR) {
                return -1;
            }
            if (*errtr == '\n') {
                *errtr = '\0';
                break;
            }
            errtr++;
            erridx++;
        }
        print_error_message(errmesg);
        free(errmesg);
        return -1;
    }
    free(status);
    tracker = NULL;

    char *checksize = malloc(sizeof(size_t));
    read_all_from_socket(socket, checksize, sizeof(size_t));
    size_t bufsize = *(size_t *)checksize;

    FILE *localfile = fopen(filename, "w+");
    size_t counter = bufsize;
    size_t acutal_get = 0;
    char *interBuf = malloc(SINGLE_BUF_SIZE);

    while (acutal_get <= bufsize) {
        ssize_t gb = read_all_from_socket(socket, interBuf, SINGLE_BUF_SIZE);
        // LOG("gb %ld\n", gb);
        if (gb == -1) {
            free(checksize);
            free(interBuf);
            exit(1);
        }
        counter -= gb;
        acutal_get += gb;
        if (gb == 0) {
            break;
        }
        fwrite(interBuf, gb, 1, localfile);
    }
    // if (counter != 0) {
    //     acutal_get += read_all_from_socket(socket, interBuf, counter);
    //     fwrite(interBuf, counter, 1, localfile);
    // }

    fprintf(stdout, "Expecting %zu bytes from server\n", bufsize);
    if (bufsize > acutal_get) {
        print_too_little_data();
    } else if (bufsize < acutal_get) {
        print_received_too_much_data();
    }
    // fprintf(stdout, "%s", fullinfo);
    fprintf(stdout, "received %zu bytes from server\n", acutal_get);

    // fflush(stdout);
    free(checksize);
    free(interBuf);
    // free(fullinfo);
    fclose(localfile);

    // fprintf(stderr, "here: %ld\n", counter);
    return 0;
}

ssize_t read_from_server(int socket, verb req) {
    // Your Code Here
    char *status = malloc(6);
    char *tracker = status;
    for (size_t i = 0; i < 6; i++) {
        int ret = read(socket, status + i, 1);
        if (ret == -1 && errno != EINTR) {
            return -1;
        }
        if (*tracker == '\n') {
            *tracker = '\0';
            break;
        }
        tracker++;
    }

    fprintf(stdout, "STATUS_%s\n", status);
    if (!strcmp(status, "ERROR")) {
        free(status);
        size_t erridx = 0;
        size_t errsize = 50;
        char *errmesg = malloc(errsize);
        char *errtr = errmesg;
        while (1) {
            if (erridx == errsize) {
                errsize *= 2;
                errmesg = realloc(errmesg, errsize);
            }
            int ret = read(socket, errmesg + erridx, 1);
            if (ret == -1 && errno != EINTR) {
                free(errmesg);
                return -1;
            }
            if (*errtr == '\n') {
                *errtr = '\0';
                break;
            }
            errtr++;
            erridx++;
        }
        print_error_message(errmesg);
        free(errmesg);
        return -1;
    }
    free(status);
    tracker = NULL;

    if (req == LIST) {
        char *checksize = malloc(sizeof(size_t));
        read_all_from_socket(socket, checksize, sizeof(size_t));

        size_t bufsize = *(size_t *)checksize;
        // LOG("checksize: %zu", bufsize);
        char *fullinfo = malloc(bufsize + 1);
        ssize_t acutalRead = read_all_from_socket(socket, fullinfo, bufsize);
        fullinfo[acutalRead] = '\0';
        fprintf(stdout, "Expecting %zu bytes from server\n", bufsize);
        fprintf(stdout, "%s", fullinfo);
        if ((ssize_t)bufsize > acutalRead) {
            print_too_little_data();
        } else if ((ssize_t)bufsize < acutalRead) {
            print_received_too_much_data();
        }
        fprintf(stdout, "received %zu bytes from server\n", acutalRead);
        fflush(stdout);
        free(checksize);
        free(fullinfo);
    } else {
        print_success();
    }

    return 0;
}



char *get_filename(int socket) {
    char *filename = malloc(256);
    char *tracker = filename;
    for (size_t i = 0; i < 256; i++) {
        int ret = read(socket, filename + i, 1);
        if (ret == -1 && errno != EINTR) {
            return NULL;
        }
        if (*tracker == '\n') {
            *tracker = '\0';
            break;
        }
        tracker++;
    }

    return filename;
}

ssize_t vector_find(vector *vec, char *str) {
    size_t vecsize = vector_size(vec);
    if (vecsize == 0) {
        return -1;
    }

    for (size_t i = 0; i < vecsize; i++) {
        char *elem = vector_get(vec, i);
        if (!strcmp(elem, str)) {
            return i;
        }
    }
    return -1;
}

size_t get_nsize(int socket) {
    size_t nsize;
    ssize_t count = sizeof(size_t);
    ssize_t counter = 0;
    while (counter != (ssize_t)count) {
        int ret_code = read(socket, &nsize + counter, count - counter);
        if (ret_code == 0) {
            break;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue;
        } else {
            perror("read");
            return -1;
        }
    }
    // LOG("counter %zd", counter);

    if (counter != sizeof(size_t)) {
        return 0;
    }
    return nsize;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    ssize_t counter = 0;
    while (counter != (ssize_t)count) {
        int ret_code = read(socket, buffer + counter, count - counter);
        if (ret_code == 0) {
            return counter;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }
    // fprintf(stderr, "here: %ld\n", counter);
    return counter;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here
    ssize_t counter = 0;
    // printf("get there\n");
    while (counter != (ssize_t)count) {
        ssize_t ret_code = write(socket, buffer + counter, count - counter);
        if (ret_code == 0) {
            return counter;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }

    return counter;
}

ssize_t write_all_to_socket_nb(int socket, const char *buffer, size_t count) {
    // Your Code Here
    ssize_t counter = 0;
    // printf("get there\n");
    while (counter != (ssize_t)count) {
        ssize_t ret_code = write(socket, buffer + counter, count - counter);
        if (ret_code == 0) {
            return counter;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)) {
            continue;
        } else {
            return -1;
        }
    }

    return counter;
}


ssize_t read_from_client(info *currinfo, vector *fv, dictionary *dict) {
    if (!dictionary_contains(dict, (void *)&(currinfo->fd))) {
        perror("dictionary_contain");
        exit(1);
    }
    currinfo = (info *)dictionary_get(dict, (void *)&(currinfo->fd));
    if (currinfo->state == 0) {
        char *myverb = header_parser(currinfo->fd);
        if (!strcmp(myverb, "PUT")) {
            free(myverb);
            currinfo->req = PUT;
        } else if (!strcmp(myverb, "GET")) {
            free(myverb);
            currinfo->req = GET;
        } else if (!strcmp(myverb, "LIST")) {
            free(myverb);
            currinfo->req = LIST;
        } else if (!strcmp(myverb, "DELETE")) {
            free(myverb);
            currinfo->req = DELETE;
        } else {
            free(myverb);
            write_all_to_socket(currinfo->fd, "ERROR\n", 6);
            write_all_to_socket(currinfo->fd, err_bad_request, strlen(err_bad_request));
            currinfo->state = 3;
            return 3;
        }
        currinfo->state = 1;

        return 1;
    }

    if (currinfo->req == GET) {
        if (currinfo->state == 1) {
            currinfo->filename = get_filename(currinfo->fd);
        }

        // size_t nominalsize = get_nsize(socket);
        nonblocking_write_to_client(currinfo);

        // LOG("state: %d", currinfo->state);
    } else if (currinfo->req == PUT) {
        if (currinfo->state == 1) {
            currinfo->filename = get_filename(currinfo->fd);
            LOG("filename is: %s", currinfo->filename);
            currinfo->nsize = get_nsize(currinfo->fd);
            // no file uploaded...
            if (currinfo->nsize == 0) {
                currinfo->state = 3;
                return 3;
            }
            LOG("nsize is: %zu", currinfo->nsize);
            if (access(currinfo->filename, F_OK) != -1) {
                FILE *truncate = fopen(currinfo->filename, "w");
                fclose(truncate);
            } else {
                vector_push_back(fv, currinfo->filename);
            }
        }
        nonblocking_read_from_client(currinfo);

        if (currinfo->state == 3) {
            LOG("nsize: %zu, offset: %zu", currinfo->nsize, currinfo->offset);
            if (currinfo->nsize > currinfo->offset) {
                write_all_to_socket(currinfo->fd, "ERROR\n", 6);
                write_all_to_socket(currinfo->fd, err_bad_file_size, strlen(err_bad_file_size));
                print_too_little_data();
                remove(currinfo->filename);
            } else if (currinfo->nsize < currinfo->offset) {
                write_all_to_socket(currinfo->fd, "ERROR\n", 6);
                write_all_to_socket(currinfo->fd, err_bad_file_size, strlen(err_bad_file_size));
                print_received_too_much_data();
                remove(currinfo->filename);
            } else {
                write_all_to_socket(currinfo->fd, "OK\n", 3);
            }
        }

    } else if (currinfo->req == DELETE) {
        char *filename = get_filename(currinfo->fd);
        ssize_t idx = vector_find(fv, filename);
        if (idx == -1) {
            write(currinfo->fd, "ERROR\n", 6);
            write(currinfo->fd, err_no_such_file, strlen(err_no_such_file));
            currinfo->state = 3;
            free(filename);
            return 3;
        }
        remove(filename);
        free(filename);
        vector_erase(fv, idx);
        write(currinfo->fd, "OK\n", 3);
        currinfo->state = 3;
    } else if (currinfo->req == LIST) {
        if (currinfo->state == 1) {
            size_t vecsize = vector_size(fv);
            write(currinfo->fd, "OK\n", 3);
            LOG("vecsize: %zu\n", vecsize);
            if (vecsize == 0) {
                write_all_to_socket(currinfo->fd, (char *)&vecsize, sizeof(size_t));
                currinfo->state = 3;
                return 0;
            }
            LOG("vecsize---: %zu\n", vecsize);
            char *filenames;
            size_t retsize = 1;

            for (size_t i = 0; i < vecsize; i++) {
                char *currfilename = vector_get(fv, i);
                LOG("cfn: %s\n", currfilename);
                size_t filenamesize = strlen(currfilename);
                if (i == 0) {
                    filenames = strdup(currfilename);
                    if (vecsize == 1) {
                        filenames = realloc(filenames, filenamesize + 1);
                        retsize += filenamesize;
                    } else {
                        filenames = realloc(filenames, filenamesize + 2);
                        strcat(filenames, "\n");
                        retsize += (filenamesize + 1);
                    }

                } else if (i == vecsize - 1) {
                    filenames = realloc(filenames, retsize + filenamesize);
                    strcat(filenames, currfilename);
                    retsize += filenamesize;
                } else {
                    filenames = realloc(filenames, retsize + filenamesize + 1);
                    strcat(filenames, currfilename);
                    strcat(filenames, "\n");
                    retsize += (filenamesize + 1);
                }
            }
            filenames[retsize - 1] = '\0';
            LOG("rs: %zu", retsize);
            retsize -= 1;
            currinfo->buf = strdup(filenames);
            free(filenames);
            write_all_to_socket(currinfo->fd, (const char *)&retsize, sizeof(size_t));
        }

        nonblocking_list_to_client(currinfo);
    } else {
        perror("verb");
        exit(1);
    }

    return 0;
}

char *header_parser(int socket) {
    char *myverb = malloc(8);
    char *tracker = myverb;
    for (size_t i = 0; i < 7; i++) {
        int ret = 0;
        while (ret != 1) {
            ret = read(socket, myverb + i, 1);
            if (ret == -1 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read");
                exit(1);
            }
        }
        if (*tracker == ' ' || *tracker == '\n') {
            *tracker = '\0';
            break;
        }
        tracker++;
    }
    myverb[7] = '\0';

    return myverb;
}

ssize_t nonblocking_write_to_client(info *currinfo) {
    // ssize_t written = currinfo->offset;
    char *tempbuf;
    size_t bufsize;
    FILE *currfile;

    currfile = fopen(currinfo->filename, "r");
    if (currfile == NULL) {
        write(currinfo->fd, "ERROR\n", 6);
        write(currinfo->fd, err_no_such_file, strlen(err_no_such_file));
        currinfo->state = 3;
        return 3;
    }
    if (currinfo->state == 1) {
        write(currinfo->fd, "OK\n", 3);
    }

    fseek(currfile, 0, SEEK_END);
    size_t fsize = ftell(currfile);
    // LOG("fsize: %zu", fsize);
    if (currinfo->state == 1) {
        write_all_to_socket_nb(currinfo->fd, (const char *)&fsize, sizeof(size_t));
    }


    if (currinfo->offset + SINGLE_BUF_SIZE > fsize) {
        bufsize = fsize - currinfo->offset;
    } else {
        bufsize = SINGLE_BUF_SIZE;
    }

    if (bufsize == 0) {
        currinfo->state = 3;
        fclose(currfile);
        return 3;
    }

    tempbuf = malloc(bufsize);

    fseek(currfile, currinfo->offset, SEEK_SET);
    fread(tempbuf, bufsize, 1, currfile);
    // tempbuf[bufsize - 1] = '\0';

    // LOG("buf is: %s, size: %zu", tempbuf, bufsize);

    ssize_t counter = 0;
    // printf("get there\n");
    while (counter != (ssize_t)bufsize) {
        ssize_t ret_code = write(currinfo->fd, tempbuf + counter, bufsize - counter);
        // LOG("ret code: %ld", ret_code);
        if (ret_code == 0) {
            currinfo->state = 3;
            free(tempbuf);
            fclose(currfile);
            return 3;
        } else if (ret_code > 0) {
            counter += ret_code;
            currinfo->offset += ret_code;
        } else if (ret_code == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                currinfo->state = 2;
                free(tempbuf);
                fclose(currfile);
                return 2;
            } else {
                free(tempbuf);
                fclose(currfile);
                return -1;
            }
        } else {
            free(tempbuf);
            fclose(currfile);
            return -1;
        }
    }

    currinfo->state = 2;
    free(tempbuf);
    fclose(currfile);
    return 2;
}

ssize_t nonblocking_list_to_client(info *currinfo) {
    char *tempbuf;
    size_t bufsize;
    char *setter = NULL;
    tempbuf = strdup(currinfo->buf);
    setter = tempbuf;
    tempbuf += currinfo->offset;
    bufsize = strlen(tempbuf);

    if (bufsize == 0) {
        currinfo->state = 3;
        free(setter);
        return 3;
    }

    ssize_t counter = 0;
    // printf("get there\n");
    while (counter != (ssize_t)bufsize) {
        ssize_t ret_code = write(currinfo->fd, tempbuf + counter, bufsize - counter);
        if (ret_code == 0) {
            currinfo->state = 3;
            free(setter);
            return 3;
        } else if (ret_code > 0) {
            counter += ret_code;
            currinfo->offset += ret_code;
        } else if (ret_code == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                currinfo->state = 2;
                free(setter);
                return 2;
            } else if (errno == EPIPE) {
                currinfo->state = 3;
                free(tempbuf);
                return -2;
            } else {
                free(setter);
                return -1;
            }
        } else {
            free(setter);
            return -1;
        }
    }

    currinfo->state = 2;
    free(setter);
    return 2;
}


ssize_t nonblocking_read_from_client(info *currinfo) {
    FILE *currfile = fopen(currinfo->filename, "a+");
    if (currfile == NULL) {
        perror("fopen");
        return 0;
    }

    size_t bufsize = SINGLE_BUF_SIZE;
    char *tempbuf = malloc(bufsize);
    if (tempbuf == NULL) {
        perror("malloc");
        exit(1);
    }
    ssize_t counter = 0;
    while (counter != (ssize_t)bufsize) {
        int ret_code = read(currinfo->fd, tempbuf + counter, bufsize - counter);
        // LOG("ret_code: %d\n", ret_code);
        if (ret_code == 0) {
            currinfo->state = 3;
            free(tempbuf);
            fclose(currfile);
            return 3;
        } else if (ret_code > 0) {
            fwrite(tempbuf + counter, ret_code, 1, currfile);
            counter += ret_code;
            currinfo->offset += ret_code;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else if (ret_code == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            currinfo->state = 2;
            free(tempbuf);
            fclose(currfile);
            return 2;
        } else {
            // fwrite(tempbuf, counter, 1, currfile);
            currinfo->state = 3;
            free(tempbuf);
            fclose(currfile);
            return -1;
        }
    }

    // fprintf(stderr, "counter size: %ld; tempbuf: %zu\n", counter, bufsize);
    currinfo->state = 2;
    // fwrite(tempbuf, counter, 1, currfile);
    free(tempbuf);
    fclose(currfile);
    return 2;
}


void info_init(info *init, int fd) {
    init->fd = fd;
    /**
     * 0: header not parsed yet...
     * 1: header parsed...
     * 2: partially read...
     * 3: done...
     */
    init->state = 0;
    init->req = 0;
    init->buf = NULL;
    init->filename = NULL;
    init->offset = 0;
    init->nsize = 0;
}

void info_destroy(info *this) {
    if (this->req == GET || this->req == PUT) {
        free(this->filename);
    }
    if (this->req == LIST) {
        free(this->buf);
    }
    free(this);
}
