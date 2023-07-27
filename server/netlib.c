#include "netlib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define handle_error(msg) \
    perror(msg); \
    exit(EXIT_FAILURE)



void DBGLG(char buf[], ...)
{
    #ifdef DEBUG
    va_list args;
    int num = 0;
    for(int i = 0; buf[i] != '\0'; ++i){
        num += buf[i] == ':';
    }
    va_start(args, buf);
    if (!num) printf("%s",buf);
    else printf("%s%d\n",buf, va_arg(args, int));

    va_end(args);
    #endif
}

void create_socket(int* sockfd, int domain, int type, int protocol)
{
    if((*sockfd = socket(domain, type, protocol)) < 0){
        handle_error("create_socket(int*, int, int, int): socket creation failed.");}
}

void create_udp_socket(int* sockfd)
{
    create_socket(sockfd, AF_INET, SOCK_DGRAM, 0);
}

void create_tcp_socket(int* sockfd)
{
    create_socket(sockfd, AF_INET, SOCK_STREAM, 0);
}

void bind_socket(int sockfd, struct sockaddr_in* addr)
{
    if (bind(sockfd, (struct sockaddr *)addr, sizeof(*addr)) < 0) {
        handle_error("bind_socket(int, struct sockaddr*): bind failed.");}
}

void listen_on_socket(int sockfd, int backlog)
{
    if(sockfd < 0 || listen(sockfd, backlog) < 0){
        handle_error("listen_on_socket(int, int): listen failed.");}
}

void update_sockaddr_in(struct sockaddr_in *addr, int family, int port, const char* s_addr)
{
    if(addr == NULL){
        handle_error("update_sockaddr_in(struct sockaddr_in*, int, int, int): NULL pointer.");}
    memset(addr, 0, sizeof(addr));
    addr->sin_family = family;
    addr->sin_port = htons(port);
    if (s_addr == NULL){
        addr->sin_addr.s_addr = INADDR_ANY;
        return;
    }
    if (inet_pton(family, s_addr, &addr->sin_addr) <= 0) {
        handle_error("update_sockaddr_in(struct sockaddr_in*, int, int, int): Failed at creating sockaddr.");}
    
    
}

int send_udp(int socku, struct sockaddr_in* sa, char* data, int len)
{
    if (socku < 0 || sendto(socku, data, len, 0, (struct sockaddr*) sa, 16) < 0){
        DBGLG("send_udp(int, struct sockaddr_in, char*, int) failed to send udp packet");
        return -1;
    }
    return 0;
}

