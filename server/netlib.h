#ifndef _NETLIB_H_INCLUDED_
#define _NETLIB_H_INCLUDED_

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>

extern void create_socket(int*, int, int,int);
extern void create_udp_socket(int*);
extern void create_tcp_socket(int*);
extern void bind_socket(int, struct sockaddr_in*);
extern void listen_on_socket(int, int);
extern void update_sockaddr_in(struct sockaddr_in*, int, int, const char*);


#endif
