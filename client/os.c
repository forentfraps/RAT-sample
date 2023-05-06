#include "includes.h"


int ScanOutPipe(HANDLE hStdoutRd, OVERLAPPED* readOverlapped, SOCKET sockt){
    DWORD bytesRead;
    char buf[2048];
    memset(&buf, 0, 2048);
    // MANUAL SIZE BUFFER KILL ME
    int c = ReadFile(hStdoutRd, buf, 2048,&bytesRead, readOverlapped);
    // WaitForSingleObjectEx(readOverlapped->hEvent, 500, TRUE);
    if (send(sockt, buf, strlen(buf), 0) < 0){
        perror("send in scanoutpipe died");
        return 1;
    }
    // WaitForSingleObjectEx(readOverlapped->hEvent, 500, TRUE);

    return 0;
}

int WriteInPipe(HANDLE hStdinWr, OVERLAPPED* writeOverlapped,  SOCKET sockt){
    DWORD bytesWritten;
    int len = 0;
    char input[1024] = {0};
    len = recv(sockt, (char*)&input, 1024, 0);
    // len <= 0 because when socket dies, recv return 0, rather than -1
    if (len <= 0){
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
