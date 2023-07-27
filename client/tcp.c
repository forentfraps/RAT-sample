#include "includes.h"

int IpSetupTCP(ADDRINFO** _ad, char* ip)
{
    ADDRINFO hints;
    int iResult;
    ZeroMemory( &hints, sizeof(hints) );
    ADDRINFO* ad;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(ip, DEFAULT_PORT_TCP, &hints, &ad);
    if ( iResult != 0 ) {
        DBGLG("IpSetupINIT failed", WSAGetLastError());
        return -1;
    }
    *_ad = ad;
    return 0;
}

int Connect(SOCKET* ConnectSocket, ADDRINFO* ad)
{
    /* Takes a pointer to an empty SOCKET and addrinto.
       Puts a hopefully connected socket to the provided pointer
       Returns 0 on success */
    int iResult;
    ADDRINFO* ptr = NULL;
    for(ptr=ad; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        *ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (*ConnectSocket == INVALID_SOCKET) {
            DBGLG("socket failed with error: ", WSAGetLastError());
            freeaddrinfo(ad);
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
    freeaddrinfo(ad);

    if (*ConnectSocket == INVALID_SOCKET) {
        DBGLG("Unable to setup the socket or connect: ", WSAGetLastError());
        return -1;
    }
    return 0;
}

int SetupSocketTCP(ADDRINFO **ad)
{
    /* Master -> Slave relationship */
    ADDRINFO hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, DEFAULT_PORT_TCP, &hints, ad) != 0){
        DBGLG("getaddrinfo failed with error: ", WSAGetLastError());
        return -1;
    }
    return 0;
}

