#include "netlib.h"
#include <pthread.h>
#include "bs_config.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>



struct hr_arg{
    struct sockaddr_in caddr;
    long unsigned int len;
    int* mutex;
};

void *handle_request(void *arg)
{
    int sockfd;
    create_udp_socket(&sockfd);
    char buf[] = "Got your message, thanks\n";
    printf("we got to a new process\n");
    struct hr_arg* args = ((struct hr_arg *) arg);
    struct sockaddr_in caddr = args->caddr;
    printf("sending\n");
    if ( sendto(sockfd, buf, sizeof(buf), 0, (struct sockaddr*)&caddr, sizeof(caddr)) <0 ){
        perror("failed to send");
    }
    *(args->mutex) = 0;
    return NULL;
}



int main(int argc, char **argv)
{
	printf("Starting\n");
    int sockfd, mutex = 1, resplen = 0;
    struct sockaddr_in addr;
    
    int port, backlog;
    char buf[BUFFER_SIZE];
    pthread_t tid;

#ifndef CUSTOM_PORT
    port = PORT;
#endif

#ifndef CUSTOM_SADDR
    char s_addr[] = "0.0.0.0";
#endif

#ifndef CUSTOM_BACKLOG
    backlog = DEFAULT_BACKLOG;
#endif
 	printf("creating udp\n");
    create_udp_socket(&sockfd);
    printf("updating sockaddr\n");
    update_sockaddr_in(&addr, AF_INET, port, s_addr);
	printf("binding socket\n");
    printf("sockfd is %d\n", sockfd);
    printf("sockaddr %p\n", &addr);
    // bind_socket(sockfd, &addr);
    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Error binding socket");
        exit(1);
    }

    struct hr_arg args = {addr, 0, &mutex};
    while(1)
    {
        struct sockaddr_in caddr;
        printf("Awaiting UDP with recvfrom\n");
        if (recvfrom(sockfd, buf, BUFFER_SIZE, 0, (struct sockaddr*)&caddr, &resplen) < 0) 
        {
            perror("Accept failed.");
            continue; 
        }
        printf("%s\n", buf);
        memcpy(&(args.caddr), &caddr, resplen);
        *(args.mutex) = 1;
        args.len = resplen;
        if(pthread_create(&tid, NULL, handle_request, &caddr) != 0)
        {
            perror("Thread creation failed.");
            continue;
        }

        pthread_detach(tid);
        while (*(args.mutex)){
            sleep(1);
            printf("waiting for mutex to unfreeze\n");

        }
    }
	printf("Exiting?\n");
    return EXIT_SUCCESS;
}
