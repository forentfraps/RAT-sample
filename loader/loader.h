#ifndef _LOADER_H
#define _LOADER_H

#include "config.h"

// #ifdef DEBUG
#include <stdio.h>
#endif

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

HMODULE _LoadLibrary(LPCWSTR lpFileName);
HMODULE _GetModuleHandle(LPCWSTR lModuleName);
FARPROC _GetProcAddress(HMODULE hModule, LPCSTR lpProcName);