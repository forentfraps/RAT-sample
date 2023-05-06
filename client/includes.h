#ifndef _INCLUDES_
#define _INCLUDES_

#include "cfg.h"
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

/* --- UTILS --- */

void DBGLG(char buf[], ...);
int MatchSig(char buf[2]);
int WinsockInitialized(void);
int InitSetup(void);


/* --- OS --- */

int ScanOutPipe(HANDLE hStdoutRd, OVERLAPPED* readOverlapped, SOCKET sockt);
int WriteInPipe(HANDLE hStdinWr, OVERLAPPED* writeOverlapped,  SOCKET sockt);


/* --- TCP --- */

int IpSetupINIT(struct addrinfo** _ad_info, char* ip);
int Connect(SOCKET* ConnectSocket, struct addrinfo* ad_info);
int SetupSocketTCP(struct addrinfo **ad_info);
int SetupServerTCP(SOCKET* ListenSocket);


/* --- UDP --- */

int SetupServerUDP(SOCKET* _socku);

#endif
