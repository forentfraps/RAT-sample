#include "includes.h"


int _SendUDP(SOCKET socku, SOCKADDR_IN* sa, char* data, int len)
{
    if (sendto(socku, data, len, 0, (SOCKADDR*) sa, 16) <= 0){
        DBGLG("Packet did not send successfully: ", WSAGetLastError());
        return -1;
    }
    return 0;
}

int SendSigUDP(SOCKET socku, SOCKADDR_IN* sa, char sig)
{
    return _SendUDP(socku, sa, &sig, 1);
}



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
    while (1){
       SendSigUDP(socku, sa, _SIG_HRBT);
       if (recvfrom(socku, buf, 1024, 0,NULL, NULL) > 0){
            return buf[0];
       }
       else{
        // DBGLG("Failed to receive: ", WSAGetLastError());
       }
    }
    return -1;
}



int InitServer(SOCKET socku, SOCKADDR_IN* sa)
{
    char buf[1024];
    int TRY_COUNT = 30;
    DBGLG("Starting initialisation with server\n");
    for (int i = 0; i < TRY_COUNT; ++i){
        SendSigUDP(socku, sa, _SIG_INIT);
        if (recvfrom(socku, buf, 1024, 0,NULL, NULL) > 0){
            if (buf[0] == _SIG_INIT){
                DBGLG("Initialisation success\n");
                return 0;
            }
            DBGLG("We recieved corrupted data back, keep trying\n");
        }
        else{
            DBGLG("recv timed out?? : ", WSAGetLastError());
        }
        Sleep(5000);
    }

    DBGLG("Initialisation failure: ", WSAGetLastError());
    return -1;
}

int SetupClientUDP(SOCKET* socku_)
{
    SOCKET socku = INVALID_SOCKET;
    if ((socku = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == INVALID_SOCKET){
        DBGLG("Failed at making a socket: ", WSAGetLastError());
        return -1;
    }
    int timeout = 30000;
    if (setsockopt(socku, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
        DBGLG("Could not set appropriate timeout in InitServer: ", WSAGetLastError());
        return -1; // Return an error code if setting the timeout fails
    }
    DBGLG("socku: ", socku);
    *socku_ = socku;
    return 0;
}

