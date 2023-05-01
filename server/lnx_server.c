#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <stdlib.h>

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

void DBGLG(char buf[]){
    #ifdef DEBUG
    printf(buf);
    #endif
}

int main(int argc, char *argv[])
{
    char ips[] = "127.0.0.1"; // :-) IP of the "infected machine"

    struct sockaddr_in servaddr;
    int ConnectSocket, iResult;
    while(1)
    /*TODO Open a UDP? socket for heartbeat connections in another thread*/
    {
        int tmp = 0;
        // Temporary choice of signature to send

        DBGLG("Connecting...\n");
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(ips);
        servaddr.sin_port = 27015;
        DBGLG("Pass 1\n");
        ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (ConnectSocket < 0){
            perror("failed to get socket desc");
            return 1;
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
