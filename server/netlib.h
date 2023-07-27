#ifndef _NETLIB_H_INCLUDED_
#define _NETLIB_H_INCLUDED_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>

#define DEBUG

void create_socket(int* sockfd, int domain, int type, int protocol);
void create_udp_socket(int* sockfd);
void create_tcp_socket(int* sockfd);
void bind_socket(int sockfd, struct sockaddr_in* addr);
void listen_on_socket(int sockfd, int backlog);
void update_sockaddr_in(struct sockaddr_in *addr, int family, int port, const char* s_addr);

void DBGLG(char buf[], ...);


#endif
