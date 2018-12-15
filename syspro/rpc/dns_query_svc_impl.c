/**
* Resplendent RPCs Lab
* CS 241 - Fall 2018
*/

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "common.h"
#include "dns_query.h"
#include "dns_query_svc_impl.h"

#define CACHE_FILE "cache_files/rpc_server_cache"

char *contact_nameserver(query *argp, char *host, int port) {
    // Your code here
    // Look in the header file for a few more comments

    // step 1
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(1);
    }

    // printf("finished 1\n");

    // step 2
    void *ret = malloc(sizeof(struct in_addr));
    int check = inet_pton(AF_INET, host, ret);
    if (check == 0 || check == -1) {
        perror("inet_pton");
        exit(1);
    }
    struct in_addr *inaddr = (struct in_addr *)ret;
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr = *inaddr;
    addr.sin_port = htons((uint16_t)port);

    // printf("argp host: %s\n", argp->host);

    // step 4
    int send_ret = sendto(sockfd, (const void *)argp->host, strlen(argp->host), 0, (struct sockaddr *)&addr, sizeof(addr));
    if (send_ret == -1) {
        perror("sendto");
        exit(1);
    }
    // printf("finished 4\n");

    // step 5
    struct sockaddr_storage newaddr;
    int newaddrlen = sizeof(newaddr);
    char *buf = malloc(MAX_BYTES_IPV4 + 1);
    int recv_count = recvfrom(sockfd, buf, MAX_BYTES_IPV4 - 1, 0, (struct sockaddr *)&newaddr, (socklen_t *)&newaddrlen);
    if (recv_count == -1) {
        perror("recvfrom");
        exit(1);
    }
    buf[recv_count] = '\0';
    // fprintf(stderr, "buf: %s\n", buf);
    // printf("finished 5\n");

    // step 6
    if (!strcmp(buf, "-1.-1.-1.-1")) {
        free(buf);
        return NULL;
    }
    return buf;
}

void create_response(query *argp, char *ipv4_address, response *res) {
    // Your code here
    // As before there are comments in the header file
    // res = malloc(sizeof(response));
    res->address = malloc(sizeof(host_address));
    // printf("cr ipv4: %s\n", ipv4_address);
    if (ipv4_address == NULL) {
        res->address->host = malloc(1);
        res->success = 0;
        res->address->host_ipv4_address = malloc(1);
    } else {
        res->address->host = strdup(argp->host);
        res->success = 1;
        res->address->host_ipv4_address = strdup(ipv4_address);
    }
}

// Stub code

response *dns_query_1_svc(query *argp, struct svc_req *rqstp) {
    printf("Resolving query...\n");
    // check its cache, 'rpc_server_cache'
    // if it's in cache, return with response object with the ip address
    char *ipv4_address = check_cache_for_address(argp->host, CACHE_FILE);
    if (ipv4_address == NULL) {
        // not in the cache. contact authoritative servers like a recursive dns
        // server
        printf("Domain not found in server's cache. Contacting authoritative "
               "servers...\n");
        char *host = getenv("NAMESERVER_HOST");
        int port = strtol(getenv("NAMESERVER_PORT"), NULL, 10);
        ipv4_address = contact_nameserver(argp, host, port);
    } else {
        // it is in the server's cache; no need to ask the authoritative
        // servers.
        printf("Domain found in server's cache!\n");
    }

    static response res;
    xdr_free(xdr_response, &res); // Frees old memory in the response struct
    create_response(argp, ipv4_address, &res);

    free(ipv4_address);

    return &res;
}
