/**
* Networking Lab
* CS 241 - Fall 2018
*/
#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close(), read()
#include <sys/epoll.h> // for epoll_create1(), epoll_ctl(), struct epoll_event
#include <string.h>    // for strncmp
#include <signal.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdio.h>

#include "vector.h"
#include "common.h"
#include "dictionary.h"

#define READ_SIZE 20000
#define SINGLE_BUF_SIZE 23000

static volatile sig_atomic_t running;
static vector *file_vector;
static dictionary *requestbook;

// TODO: read_from_client

void iwannaquit() {
    running = 0;
}

int main(int argc, char **argv) {
    // good luck!
    if (argc != 2) {
        print_server_usage();
        exit(1);
    }

    struct sigaction quit;
    memset(&quit, 0, sizeof(quit));
    quit.sa_handler = iwannaquit;
    quit.sa_flags = 0;
    sigemptyset(&quit.sa_mask);
    sigaction(SIGINT, &quit, NULL);
    signal(SIGPIPE, SIG_IGN);

    int sock_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int s = getaddrinfo(NULL, argv[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    int enable = 1;
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(1);
    }

    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind()");
        exit(1);
    }

    if (listen(sock_fd, SOMAXCONN) != 0) {
        perror("listen()");
        exit(1);
    }

    // create a temp dir for server...
    char template[] = "XXXXXX";
    char *tempdir = mkdtemp(template);
    if (tempdir == NULL) {
        perror("mkdtemp()");
        exit(1);
    }
    print_temp_directory(tempdir);
    if (chdir(tempdir) == -1) {
        perror("chdir()");
        exit(1);
    }

    running = 1;

    file_vector = string_vector_create();
    requestbook = int_to_shallow_dictionary_create();

    int epfd = epoll_create(1);
    if (epfd == -1) {
        fprintf(stderr, "Failed to create epoll file descriptor\n");
        return 1;
    }
    struct epoll_event event;
    info *myinfo = malloc(sizeof(info));
    memset(myinfo, 0, sizeof(info));
    // info_init(myinfo, sock_fd);
    info_init(myinfo, sock_fd);
    dictionary_set(requestbook, (void *)&sock_fd, myinfo);

    event.events = EPOLLIN;
    event.data.ptr = myinfo;

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock_fd, &event) == -1) {
        fprintf(stderr, "Failed to add file descriptor to epoll\n");
        close(epfd);
        return 1;
    }

    while (running) {
        if (epoll_wait(epfd, &event, 1, 2000) > 0) {
            info *currinfo = event.data.ptr;
            if (currinfo->fd == sock_fd) {
                // init_conn = 1;
                int client_fd = accept(sock_fd, NULL, NULL);
                int flags = fcntl(client_fd, F_GETFL, 0);
                fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
                info *nextinfo = malloc(sizeof(info));
                memset(nextinfo, 0, sizeof(info));
                info_init(nextinfo, client_fd);
                dictionary_set(requestbook, (void *)&(nextinfo->fd), nextinfo);
                event.data.ptr = nextinfo;
                event.events = EPOLLIN;
                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &event);

            } else {
                read_from_client(currinfo, file_vector, requestbook);
                if (currinfo->state != 3) {
                    event.events = EPOLLOUT;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, currinfo->fd, &event);
                } else {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, currinfo->fd, NULL);
                    shutdown(currinfo->fd, SHUT_RDWR);
                    close(currinfo->fd);
                    dictionary_remove(requestbook, (void*)&(currinfo->fd));
                    info_destroy(currinfo);
                }
            }
        }
    }

    fprintf(stdout, "Shutting down...\n");
    if (close(epfd)) {
        fprintf(stderr, "Failed to close epoll file descriptor\n");
        return 1;
    }
    for (size_t i = 0; i < vector_size(file_vector); i++) {
        unlink(vector_get(file_vector, i));
    }
    if (chdir("..") == -1) {
        perror("chdir()");
        exit(1);
    }
    rmdir(tempdir);
    dictionary_destroy(requestbook);
    vector_destroy(file_vector);
    info_destroy(myinfo);
    free(result);
    if (shutdown(sock_fd, SHUT_RDWR) != 0) {
        perror("shutdown");
    }
    close(sock_fd);


    return 0;
}
