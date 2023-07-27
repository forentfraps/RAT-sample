#include "netlib.h"
#include <pthread.h>
#include "bs_config.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char **argv)
{
	printf("Starting\n");
    int sockfd_u, csockfd,resplen = 0;
    struct sockaddr_in addr;
    ssize_t bytesRead;
    int port, backlog;
    char buf[BUFFER_SIZE];
    pthread_t tid;
    char ip4[INET_ADDRSTRLEN];
    struct sockaddr_in caddr;
#ifndef CUSTOM_PORT
    port = PORT;
#endif

#ifndef CUSTOM_SADDR
    char s_addr[] = "0.0.0.0";
#endif

#ifndef CUSTOM_BACKLOG
    backlog = DEFAULT_BACKLOG;
#endif
    int recv_socket, response_socket;
    struct sockaddr_in local_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[1024];

    create_udp_socket(&sockfd_u);
    update_sockaddr_in(&local_addr, AF_INET, htons(PORT), NULL);
    bind_socket(sockfd_u, &local_addr);

    // TODO: Start a thread which listens for UDP
    // and adds new clients to some kind of DB
    DBGLG("Listening for UDP packets...\n");
    while (1) {
        // TODO: Write a menu
        // basically a control panel
        // It can choose to which client to interact
        // Or Interact with all



        // if(pthread_create(&tid, NULL, handle_request, &caddr) != 0)
        // {
        //     perror("Thread creation failed.");
        //     continue;
        // }

        // pthread_detach(tid);
    }
	printf("Exiting?\n");
    return EXIT_SUCCESS;
}
