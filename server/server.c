#include "netlib.h"
#include <pthread.h>
#include "bs_config.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void *handle_request(void *arg)
{
    int client_fd = *((int *) arg);
    char request[BUFFER_SIZE];
    char response[] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Hello, world!</h1></body></html>";
    
    
    ssize_t bytes_received = recv(client_fd, request, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("handle_request: receive.");
        close(client_fd);
        return NULL;
    }

    ssize_t bytes_sent = send(client_fd, response, strlen(response), 0);
    if (bytes_sent < 0) {
        perror("handle_request: send.");
    }
    
    
    close(client_fd);
    return NULL;
}

int main(int argc, char *argv)
{
    int sockfd, clientfd;
    struct sockaddr_in addr;
    int port, s_addr, backlog;
    pthread_t tid;

#ifndef CUSTOM_PORT
    port = PORT;
#endif

#ifndef CUSTOM_SADDR
    s_addr = INADDR_ANY;
#endif

#ifndef CUSTOM_BACKLOG
    backlog = DEFAULT_BACKLOG;
#endif

    create_udp_socket(&sockfd);
    update_sockaddr_in(&addr, AF_INET, port, s_addr);

    bind_socket(sockfd, (struct sockaddr*)&addr);
    listen_on_socket(sockfd, backlog);
    
    while(1)
    {
        if ((clientfd = accept(sockfd, (struct sockaddr *) &addr, (socklen_t*)sizeof(addr))) < 0) 
        {
            perror("Accept failed.");
            continue;
        }

        if(pthread_create(&tid, NULL, handle_request, &clientfd) != 0)
        {
            perror("Thread creation failed.");
            close(clientfd);
            continue;
        }

        pthread_detach(tid);
    }

    return EXIT_SUCCESS;
}
