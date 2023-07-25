#include "netlib.h"
#include <pthread.h>
#include "bs_config.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



struct hr_arg{
    struct sockaddr_in caddr;
    long unsigned int len;
};

// void *handle_request(void *arg)
// {
//     int sockfd;
//     struct hr_arg* args = ((struct hr_arg *) arg);
//     struct sockaddr_storage caddr = args->caddr;
//     long unsigned int len = args->len;
//     create_udp_socket(&sockfd);
//     char buf[] = "Got your message, thanks\n";
//     printf("we got to a new process\n");
//     printf("sending\n");
//     if ( sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&caddr, 8) <0 ){
//         perror("failed to send");
//     }
//     printf("We sent it\n");
//     printf("address for mutex in thread == %p\n", (args->mutex));
//     return NULL;
// }



int main(int argc, char **argv)
{
	printf("Starting\n");
    int sockfd, csockfd,resplen = 0;
    struct sockaddr_in addr;
    ssize_t bytesRead;
    int port, backlog;
    char buf[BUFFER_SIZE];
    pthread_t tid;
    char ip4[INET_ADDRSTRLEN];
    // struct hr_arg args;
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

    // Create a UDP socket for receiving data
    recv_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recv_socket < 0) {
        perror("Error creating recv socket");
        return 1;
    }

    // Bind the socket to a specific address and port to receive data
    memset(&local_addr, 0, sizeof(local_addr));
    local_addr.sin_family = AF_INET;
    local_addr.sin_addr.s_addr = INADDR_ANY;
    local_addr.sin_port = htons(PORT);

    if (bind(recv_socket, (struct sockaddr *)&local_addr, sizeof(local_addr)) < 0) {
        perror("Error binding");
        return 1;
    }

    printf("Listening for UDP packets...\n");
    char RESPONSE_MESSAGE[] = "fun\n";
    while (1) {
        // Receive data and the sender's address (client_addr) and port
        int received_bytes = recvfrom(recv_socket, buffer, sizeof(buffer), 0,
                                      (struct sockaddr *)&client_addr, &client_addr_len);

        if (received_bytes < 0) {
            perror("Error receiving data");
            break;
        }

        // Process the received data if needed
        buffer[received_bytes] = '\0';
        // client_addr.sin_port = htons(55555);
        // Create a UDP socket for sending the response
        response_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (response_socket < 0) {
            perror("Error creating response socket");
            break;
        }
        // Send back a UDP response
        received_bytes = sendto(recv_socket, RESPONSE_MESSAGE, strlen(RESPONSE_MESSAGE), 0,
               (struct sockaddr *)&client_addr, client_addr_len);
        if (received_bytes < 0){
            perror("Error sending data");
        }
        printf("We have sent %d\n",received_bytes);

        // Close the response socket
        close(response_socket);
        // memcpy(&(args.caddr), &caddr, sizeof(caddr));
        // printf("address for mutex in thread == %p\n", (args.mutex));
        // args.len = resplen;
        // mutex = 1;
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
