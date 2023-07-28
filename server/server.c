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
                    if (add_db(db, (char*)&client_addr, time(NULL)) == 0){
                        if((sendto(sockfd_u, &siginit, 1, 0, (struct sockaddr*)&client_addr, 16))< 0){
                            DBGLG("Failed at sending data\n");
                        }
                    }
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

int main(int argc, char **argv)
{
	printf("Starting\n");
    pthread_t tid;
    struct db* db = NULL;
    int sockfd_u;
    struct sockaddr_in local_addr;
    
    create_db(&db);
    if (db == NULL){
        exit(EXIT_FAILURE);
    }
    create_udp_socket(&sockfd_u);
    
    update_sockaddr_in(&local_addr, AF_INET, PORT, NULL);
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
                printf("UNIMPLEMENTED\n");
                break;
            case 2:
                terminate_thread = 1;
                pthread_join(tid, NULL);
                save_db(db, "./db.db");
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
