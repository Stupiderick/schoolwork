/**
* Networking Lab
* CS 241 - Fall 2018
*/

#pragma once
#include <stddef.h>
#include <sys/types.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "format.h"
#include "vector.h"
#include "dictionary.h"



static const size_t MESSAGE_SIZE_DIGITS = 4;

#define LOG(...)                      \
    do {                              \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n");        \
    } while (0);

typedef enum { GET, PUT, DELETE, LIST, V_UNKNOWN } verb;

typedef struct info {
    int fd;
    int state;
    verb req;
    void *buf;
    char *filename;
    size_t offset;
    size_t nsize;
} info;

void info_init(info *init, int fd);
void info_destroy(info *this);
ssize_t get_from_server(int socket, char *filename);
ssize_t read_from_server(int socket, verb req);
ssize_t read_from_client(info *currinfo, vector *fv, dictionary *dict);
ssize_t read_all_from_socket(int socket, char *buffer, size_t count);
ssize_t write_all_to_socket(int socket, const char *buffer, size_t count);
ssize_t write_all_to_socket_nb(int socket, const char *buffer, size_t count);
ssize_t vector_find(vector *vec, char *str);
char *get_filename(int socket);
size_t get_nsize(int socket);
char *header_parser(int socket);
ssize_t nonblocking_write_to_client(info *currinfo);
ssize_t nonblocking_list_to_client(info *currinfo);
ssize_t nonblocking_read_from_client(info *currinfo);
