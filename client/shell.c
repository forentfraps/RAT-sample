#include <stdio.h>
#include <windows.h>

int main()
{
    HANDLE hStdinRd, hStdinWr, hStdoutRd, hStdoutWr;
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };

    // Create pipes for the child process's standard input and output.
    if (!CreatePipe(&hStdoutRd, &hStdoutWr, &sa, 0) ||
        !CreatePipe(&hStdinRd, &hStdinWr, &sa, 0))
    {
        fprintf(stderr, "Error creating pipes\n");
        return 1;
    }

    // Set the bInheritHandle flag so the child process inherits the handles.
    sa.bInheritHandle = TRUE;

    // Create the child process.
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = hStdinRd;
    si.hStdOutput = hStdoutWr;
    if (!CreateProcess(NULL, "C:\\Windows\\System32\\cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
    {
        fprintf(stderr, "Error creating child process\n");
        return 1;
    }

    // Close the write ends of the pipes in the parent process.
    CloseHandle(hStdoutWr);
    CloseHandle(hStdinRd);

    // Read from the output pipe and write to the input pipe.
    here:
    char buffer[1024];
    DWORD bytesRead;
    int c = 0;
    while ((c = ReadFile(hStdoutRd, buffer, sizeof(buffer), &bytesRead, NULL)) && bytesRead > 0)
    {
        // Do something with the output from the child process.
        printf("we are here\n");
        fwrite(buffer, 1, bytesRead, stdout);

        // Send some input to the child process.
        // const char* input = "dir\n";

    }
    printf("writing?\n");
    char input[128] = {0};
    scanf("%s", &input);
    DWORD bytesWritten;
    WriteFile(hStdinWr, input, strlen(input), &bytesWritten, NULL);
    goto here;

    // Close the read end of the output pipe.
    CloseHandle(hStdoutRd);

    // Wait for the child process to exit.
    WaitForSingleObject(pi.hProcess, INFINITE);

    return 0;
}