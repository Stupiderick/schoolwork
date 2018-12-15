/**
* Networking Lab
* CS 241 - Fall 2018
*/

#include "common.h"
#include "format.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define SINGLE_BUF_SIZE 30000

char **parse_args(int argc, char **argv);
verb check_args(char **args);


int main(int argc, char **argv) {
    // Good luck!
    verb mtd = check_args(argv);
    char **parsed = parse_args(argc, argv);

    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo(parsed[0], parsed[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        free(parsed);
        free(result);
        exit(1);
    }

    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("connect");
        free(parsed);
        free(result);
        exit(2);
    }

    char *buf;
    size_t bufsize = 0;

    if (mtd == GET) {
        bufsize = strlen(parsed[2]) + strlen(parsed[3]) + 3;
        buf = malloc(bufsize);
        sprintf(buf, "%s %s\n", parsed[2], parsed[3]);
        buf[bufsize - 1] = '\0';
        write_all_to_socket(sock_fd, buf, bufsize);
        shutdown(sock_fd, SHUT_WR);
        // read_from_server(sock_fd, GET);
        get_from_server(sock_fd, parsed[4]);

    } else if (mtd == PUT) {
        bufsize = strlen(parsed[2]) + strlen(parsed[3]) + 3;
        buf = malloc(bufsize);
        sprintf(buf, "%s %s\n", parsed[2], parsed[3]);
        write_all_to_socket(sock_fd, buf, bufsize - 1);

        FILE *localfile = fopen(parsed[4], "r+");
        if (localfile == NULL) {
            free(result);
            free(parsed);
            free(buf);
            print_error_message((char *)err_no_such_file);
            exit(1);
        }
        // int fd = fileno(localfile);
        fseek(localfile, 0L, SEEK_END);
        size_t fs = ftell(localfile);
        fseek(localfile, 0L, SEEK_SET);
        size_t countdown = fs;
        write_all_to_socket(sock_fd, (char *)&fs, sizeof(size_t));

        // LOG("fs: %zu\n", fs);
        char *interBuf = malloc(SINGLE_BUF_SIZE);
        while (countdown >= SINGLE_BUF_SIZE) {
            fread(interBuf, SINGLE_BUF_SIZE, 1, localfile);
            countdown -= write_all_to_socket(sock_fd, interBuf, SINGLE_BUF_SIZE);
        }
        if (countdown != 0) {
            fread(interBuf, countdown, 1, localfile);
            write_all_to_socket(sock_fd, interBuf, countdown);
        }
        free(interBuf);
        fclose(localfile);

        shutdown(sock_fd, SHUT_WR);
        read_from_server(sock_fd, PUT);
    } else if (mtd == LIST) {
        bufsize = 6;
        buf = malloc(bufsize);
        sprintf(buf, "LIST\n");
        buf[5] = '\0';
        write_all_to_socket(sock_fd, buf, bufsize);
        shutdown(sock_fd, SHUT_WR);
        read_from_server(sock_fd, LIST);
    } else if (mtd == DELETE) {
        bufsize = strlen(parsed[2]) + strlen(parsed[3]) + 3;
        buf = malloc(bufsize);
        sprintf(buf, "%s %s\n", parsed[2], parsed[3]);
        buf[bufsize - 1] = '\0';
        write_all_to_socket(sock_fd, buf, bufsize);
        shutdown(sock_fd, SHUT_WR);
        read_from_server(sock_fd, DELETE);
        // fprintf(stderr, "buf is %s\n", buf);
    } else {
        exit(2);
    }

    free(result);
    free(parsed);
    free(buf);

    return 0;
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
    if (argc < 3) {
        return NULL;
    }

    char *host = strtok(argv[1], ":");
    char *port = strtok(NULL, ":");
    if (port == NULL) {
        return NULL;
    }

    char **args = calloc(1, 6 * sizeof(char *));
    args[0] = host;
    args[1] = port;
    args[2] = argv[2];
    char *temp = args[2];
    while (*temp) {
        *temp = toupper((unsigned char)*temp);
        temp++;
    }
    if (argc > 3) {
        args[3] = argv[3];
    }
    if (argc > 4) {
        args[4] = argv[4];
    }

    return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
    if (args == NULL) {
        print_client_usage();
        exit(1);
    }

    char *command = args[2];

    if (strcmp(command, "LIST") == 0) {
        return LIST;
    }

    if (strcmp(command, "GET") == 0) {
        if (args[3] != NULL && args[4] != NULL) {
            return GET;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "DELETE") == 0) {
        if (args[3] != NULL) {
            return DELETE;
        }
        print_client_help();
        exit(1);
    }

    if (strcmp(command, "PUT") == 0) {
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            exit(1);
        }
        return PUT;
    }

    // Not a valid Method
    print_client_help();
    exit(1);
}
