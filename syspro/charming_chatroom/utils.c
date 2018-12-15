/**
* Chatroom Lab
* CS 241 - Fall 2018
*/

// partnered with luo34

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes = read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    // printf("ret is: %ld\n", read_bytes);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here
    size_t newsize = ntohl(size);
    ssize_t write_bytes = write_all_to_socket(socket, (char *)&newsize, MESSAGE_SIZE_DIGITS);


    return write_bytes;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here
    ssize_t counter = 0;
    while (counter != (ssize_t)count) {
        int ret_code = read(socket, buffer + counter, count - counter);
        if (ret_code == 0) {
            return 0;
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
            return 0;
        } else if (ret_code > 0) {
            counter += ret_code;
        } else if (ret_code == -1 && errno == EINTR) {
            continue;
        } else {
            return -1;
        }
    }
    // fprintf(stderr, "write here: %ld\n", counter);
    return counter;
}
