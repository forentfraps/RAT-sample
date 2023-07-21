#include "includes.h"

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
