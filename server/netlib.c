#include "netlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define handle_error(msg) \
    perror(msg); \
    exit(EXIT_FAILURE)

void create_socket(int* sockfd, int domain, int type, int protocol)
{
    if((*sockfd = socket(domain, type, protocol)) < 0)
        handle_error("create_socket(int*, int, int, int): socket creation failed.");
}

void create_udp_socket(int* sockfd)
{
    create_socket(sockfd, AF_INET, SOCK_DGRAM, 0);
}

void create_tcp_socket(int* sockfd)
{
    create_socket(sockfd, AF_INET, SOCK_STREAM, 0);
}

void bind_socket(int sockfd, struct sockaddr* addr)
{
    if(sockfd < 0 || bind(sockfd, (const struct sockaddr*)addr, (socklen_t)sizeof(addr)) < 0)
        handle_error("bind_socket(int, struct sockaddr*): bind failed.");
}

void listen_on_socket(int sockfd, int backlog)
{
    if(sockfd < 0 || listen(sockfd, backlog) < 0)
        handle_error("listen_on_socket(int, int): listen failed.");
}

void update_sockaddr_in(struct sockaddr_in *addr, int family, int port, int s_addr)
{
    if(addr == NULL)
        handle_error("update_sockaddr_in(struct sockaddr_in*, int, int, int): NULL pointer.");
    memset(addr, 0, sizeof(addr));

    addr->sin_family = family;
    addr->sin_addr.s_addr = s_addr;
    addr->sin_port = htons(port);
}