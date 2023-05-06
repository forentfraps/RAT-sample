#include "includes.h"

int IpSetupINIT(struct addrinfo** _ad_info, char* ip)
{
    /* Slave -> Master relation ship */
    struct addrinfo hints;
    int iResult;
    ZeroMemory( &hints, sizeof(hints) );
    struct addrinfo* ad_info;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, DEFAULT_PORT_TCP, &hints, &ad_info);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return -5;
    }
    *_ad_info = ad_info;
    return 0;
}

int Connect(SOCKET* ConnectSocket, struct addrinfo* ad_info)
{
    /* Slave -> Master relationship
       Takes a pointer to an empty SOCKET and addrinto.
       Puts a hopefully connected socket to the provided pointer
       Returns >= 0 on success */
    int iResult;
    struct addrinfo* ptr = NULL;
    for(ptr=ad_info; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        *ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (*ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(ad_info);
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
    freeaddrinfo(ad_info);

    if (*ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        return -1;
    }
    return 0;
}

int SetupSocketTCP(struct addrinfo **ad_info){
    /* Master -> Slave relationship */
    int iResult;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    iResult = getaddrinfo(NULL, DEFAULT_PORT_TCP, &hints, ad_info);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return -1;
    }
    return 0;
}

int SetupServerTCP(SOCKET* ListenSocket){
    int iResult;
    struct addrinfo *ad_info = NULL;

    if (!WinsockInitialized()){
        iResult = InitSetup();
        if (iResult < 0){
            return iResult;
        }
    }
    iResult = SetupSocketTCP(&ad_info);
    if (iResult < 0){
        perror("socket setup fail");
        return -1;
    }
    *ListenSocket = socket(ad_info->ai_family, ad_info->ai_socktype, ad_info->ai_protocol);
    if (*ListenSocket == INVALID_SOCKET) {
        DBGLG("socket failed with error: \n", WSAGetLastError());
        freeaddrinfo(ad_info);
        WSACleanup();
        return -1;
    }
    iResult = bind( *ListenSocket, ad_info->ai_addr, (int)ad_info->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        DBGLG("bind failed with error: \n", WSAGetLastError());
        freeaddrinfo(ad_info);
        closesocket(*ListenSocket);
        WSACleanup();
        return -1;
    }
    freeaddrinfo(ad_info);

    iResult = listen(*ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        DBGLG("listen failed with error: \n", WSAGetLastError());
        closesocket(*ListenSocket);
        WSACleanup();
        return -1;
    }
    return 0;
}
