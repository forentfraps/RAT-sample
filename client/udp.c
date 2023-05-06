#include "includes.h"

int SetupServerUDP(SOCKET* _socku){
    struct sockaddr_in local;
    SOCKET socku = INVALID_SOCKET;
    *_socku = socku;
    int iResult;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(27016);
    socku = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    printf("%d", WSAGetLastError());
    if (socku == INVALID_SOCKET){
        perror("failed creating socket");
        return 1;

    }
    printf("%d", WSAGetLastError());
    iResult = bind(socku, (struct sockaddr *)&local, sizeof(local));
    if (iResult < 0){
        perror("failed to bind a socket");
        return 2;
        // TODO HANDLE INVALID PORT BIND
    }
    printf("%d", WSAGetLastError());
    *_socku = socku;
    return 0;
}