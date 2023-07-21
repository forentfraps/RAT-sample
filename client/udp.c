#include "includes.h"


int SetupHeartBeatUDP(SOCKET* _socku, struct sockaddr_in* _ad_info, const char* ip)
{
    SOCKET socku = INVALID_SOCKET;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // use IPv4
    hints.ai_socktype = SOCK_DGRAM; // use UDP
    if ((rv = getaddrinfo(ip, NULL, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rv));
        return -1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        (*(struct sockaddr_in *)p->ai_addr).sin_port= 8888;
        if ((socku = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }


        break;
    }
    if (socku < 0){
        perror("failed at creating socket");
        return -1;
    }
    *_socku = socku;
    *_ad_info = *(struct sockaddr_in *)p->ai_addr;
    freeaddrinfo(servinfo);
    return 0;
}


DWORD WINAPI _HeartBeat(LPVOID lpParam)
{
    char* ip = (char*) lpParam;
    SOCKET socku = INVALID_SOCKET;
    struct sockaddr_in ad_info;
    int iResult = SetupHeartBeatUDP(&socku, &ad_info, ip);
    printf("ip we got %s\n", inet_ntoa(ad_info.sin_addr));
    if (iResult < 0){
        perror("failed udp setup.");
        return -1;
    }
    while (1){
        printf("%x %x \n", _SIG_HRBT[0], _SIG_HRBT[1] );
        iResult = sendto(socku, "hi", 3, 0, (struct sockaddr*) &ad_info, sizeof(ad_info)) ;
        DBGLG("Sent heartbeat: \n", iResult);
        DBGLG("WSA: \n", WSAGetLastError());
        // Sleep((60*10 + rand(5)*60) * 1000);
        Sleep(5000);
    }

    return 0;
}



int SetupServerUDP(SOCKET* _socku)
{
    struct sockaddr_in local;
    SOCKET socku = INVALID_SOCKET;
    *_socku = socku;
    int iResult;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(DEFAULT_PORT_UDP);
    socku = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socku == INVALID_SOCKET){
        perror("failed creating socket");
        return -1;
    }

    iResult = bind(socku, (struct sockaddr *)&local, sizeof(local));
    if (iResult < 0){
        perror("failed to bind a socket");
        return -2;
        // TODO HANDLE INVALID PORT BIND
    }
    *_socku = socku;
    return 0;
}


int HeartBeat(const char* ip, HANDLE* killme){
    int iResult;
    HANDLE hThread;
    DWORD threadId;
    hThread = CreateThread(NULL, 0, _HeartBeat, (LPVOID)ip, 0, &threadId);
    if (hThread == NULL) {
        printf("Error creating thread.\n");
        return 1;
    }
    printf("1\n");
    *killme = hThread;
    printf("2\n");
    return 0;
}
