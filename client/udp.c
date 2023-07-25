#include "includes.h"




int GenerateSockaddrin(SOCKADDR_IN* ptr, char* ip, short port)
{
    memset(ptr, 0, sizeof(*ptr));
    ptr->sin_family = AF_INET;
    ptr->sin_port = htons(port);
    ptr->sin_addr.s_addr = inet_addr(ip);
    if (ptr->sin_addr.s_addr == INADDR_NONE){
        DBGLG("Bad ip passed in GenerateSockaddrin: ", WSAGetLastError());
        return -1;
    }
    return 0;
}

int HeartBeat(SOCKET socku, SOCKADDR_IN* sa)
// Sends a HeartBeat every second, returns possible response, -1 theoretically unreachable
{
    char buf[1024];
    FD_SET fds;
    TIMEVAL tv;
    FD_ZERO(&fds);
    FD_SET(socku, &fds);
    tv.tv_sec = 1;
    while (1){
       SendUDP(socku, sa, _SIG_HRBT, 1);
       if (select(0, &fds, NULL, NULL, &tv) > 0){
            recvfrom(socku, buf, 1024, 0,NULL, NULL);
            return buf[0];
       }
    }
    return -1;
}

int SetupClientUDP(SOCKET* socku_)
{
    SOCKET socku = INVALID_SOCKET;
    if (socku = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP) <= 0){
        DGBLG("Failed at making a socket: ", WSAGetLastError());
        return -1;
    }
    DBGLG("socku %p", socku);
    *socku_ = socku;
    return 0;
}

int SendUDP(SOCKET socku, SOCKADDR_IN* sa, char* data, int len)
{
    if (sendto(socku, data, len, 0, (SOCKADDR*) sa, 16) != len){
        DBGLG("Packet did not send successfully: ", WSAGetLastError());
        return -1;
    }
    return 0;
}

