#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#define DEFAULT_PORT 27015
#define DEBUG


short SocketCreate(void)
{
    /* Master -> Slave relationship*/
    short hSocket;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}
int BindCreatedSocket(int hSocket)
{
    /* Master -> Slave relationship
    Server needs to host a tcp socket to accept heartbeats */
    int iRetval=-1;
    int ClientPort = 27015;
    struct sockaddr_in  remote= {0};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = htonl(INADDR_ANY);
    remote.sin_port = htons(ClientPort); /* Local port */
    iRetval = bind(hSocket,(struct sockaddr *)&remote,sizeof(remote));
    return iRetval;
}

int ParseInto(unsigned char* buf, int len, char* name){
    FILE* fd = fopen(name,"w");
    if(!fd) {
        free(buf);
        return -1;
    }
    int res = fwrite(buf,1, (size_t) len, fd);
    printf("%d", res);
    fclose(fd);
    free(buf);
    return 0;
}

void DBGLG(char buf[], ...){
    #ifdef DEBUG
    va_list args;
    int num = 0;
    for(int i = 0; buf[i] != '\0'; ++i){
        num += buf[i] == ':';
    }
    va_start(args, buf);
    if (!num) printf("%s",buf);
    else printf("%s%d\n",buf, va_arg(args, int));

    va_end(args);
    #endif
}

int main(int argc, char *argv[])
{
    char ips[] = "172.29.176.1"; // :-) IP of the "infected machine"

    struct sockaddr_in servaddr;
    int ConnectSocket = 0, iResult;
    while(1)
    /*TODO Open a UDP? socket for heartbeat connections in another thread*/
    {
        int tmp = 0;
        // Temporary choice of signature to send

        DBGLG("Connecting...\n");
        memset(&servaddr, '0', sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        // servaddr.sin_addr.s_addr = inet_addr(ips);
        servaddr.sin_port = htons(27015);
        DBGLG("Pass 1\n");
        ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (ConnectSocket < 0){
            perror("failed to get socket desc");
            return 1;
        }
        iResult = inet_pton(AF_INET, ips, &servaddr.sin_addr);
        if (iResult <= 0){
            perror("inet_pton failed");
        }
        DBGLG("Pass 2\n");
        iResult = connect(ConnectSocket, (struct sockaddr* ) &servaddr, sizeof(servaddr));
        if(iResult < 0){
            perror("failed to connect");
            return -1;
        }
        DBGLG("Pass 3\n");
        char txt[2] = {0xfb, tmp};
        iResult = send(ConnectSocket, txt, 2, 0);
        if (iResult < 0){
            perror("fail send");
            return 1;
        }

        close(ConnectSocket);
        break;

    }
    return 0;
}
