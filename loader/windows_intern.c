#include "loader.h"

unsigned short ntdll_char[] = L"C:\\Windows\\SYSTEM32\\ntdll.dll";

typedef VOID(NTAPI* pRtlInitUnicodeString)(PUNICODE_STRING DestinationString, PCWSTR SourceString);
wchar_t* custom_wcsncpy(wchar_t* dest, const wchar_t* src, size_t count) {
    wchar_t* originalDest = dest;
    while (count > 0) {
        *dest = *src;
        if (*src == L'\0') {
            break;
        }
        dest++;
        src++;
        count--;
    }
    while (count > 0) {
        *dest = L'\0';
        dest++;
        count--;
    }
    return originalDest;
}
size_t custom_wcslen(const wchar_t* str) {
    if (str == nullptr) {
        return 0;
    }
    size_t length = 0;
    while (*str != L'\0') {
        length++;
        str++;
    }
    return length;
}
int custom_wcscmp(const wchar_t* str1, const wchar_t* str2) {
    while (*str1 == *str2 && *str1 != L'\0') {
        str1++;
        str2++;
    }
    return *str1 - *str2;
}
errno_t custom_wcscpy_s(wchar_t* dest, size_t destSize, const wchar_t* src) {
    if (dest == nullptr || src == nullptr) {
        return EINVAL;
    }
    size_t srcLength = custom_wcslen(src);
    if (destSize < srcLength + 1) {
        return ERANGE;
    }
    custom_wcsncpy(dest, src, destSize);
    dest[srcLength] = L'\0';
    return 0;
}
int custom_strcmp(const char* str1, const char* str2) {
    while (*str1 || *str2) {
        if (*str1 < *str2) {
            return -1;
        }
        else if (*str1 > *str2) {
            return 1;
        }
        str1++;
        str2++;
    }
    return 0;
}
FARPROC _GetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule +
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    DWORD* addressOfFunctions = (DWORD*)((BYTE*)hModule + exportDirectory->AddressOfFunctions);
    WORD* addressOfNameOrdinals = (WORD*)((BYTE*)hModule + exportDirectory->AddressOfNameOrdinals);
    DWORD* addressOfNames = (DWORD*)((BYTE*)hModule + exportDirectory->AddressOfNames);
    for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
        if (custom_strcmp(lpProcName, (const char*)hModule + addressOfNames[i]) == 0) {
            return (FARPROC)((BYTE*)hModule + addressOfFunctions[addressOfNameOrdinals[i]]);
        }
    }
    return NULL;
}
void* __cdecl custom_memset(void* Destination, int Value, size_t Size) {
    unsigned char* p = (unsigned char*)Destination;
    while (Size > 0) {
        *p = (unsigned char)Value;
        p++;
        Size--;
    }
    return Destination;
}
HMODULE _GetModuleHandle(LPCWSTR lModuleName) {
    PEB* pPeb = (PEB*)__readgsqword(0x60);
    // For x86
    // PEB* pPeb = (PEB*)__readgsqword(0x30);
    PEB_LDR_DATA* Ldr = pPeb->Ldr;
    LIST_ENTRY* ModuleList = &Ldr->InMemoryOrderModuleList;
    LIST_ENTRY* pStartListEntry = ModuleList->Flink;
    WCHAR mystr[MAX_PATH] = { 0 };
    WCHAR substr[MAX_PATH] = { 0 };
    for (LIST_ENTRY* pListEntry = pStartListEntry; pListEntry != ModuleList; pListEntry = pListEntry->Flink) {
        LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)((BYTE*)pListEntry - sizeof(LIST_ENTRY));
        custom_memset(mystr, 0, MAX_PATH * sizeof(WCHAR));
        custom_memset(substr, 0, MAX_PATH * sizeof(WCHAR));
        custom_wcscpy_s(mystr, MAX_PATH, pEntry->FullDllName.Buffer);
        custom_wcscpy_s(substr, MAX_PATH, lModuleName);
        if (custom_wcscmp(substr, mystr) == 0) {
            // Returning the DLL base address
            return (HMODULE)pEntry->DllBase;
        }
    }
}
HMODULE _LoadLibrary(LPCWSTR lpFileName) {
    UNICODE_STRING ustrModule;
    HANDLE hModule = NULL;
    HMODULE hNtdll = _GetModuleHandle(ntdll_char);
    pRtlInitUnicodeString RtlInitUnicodeString = (pRtlInitUnicodeString)_GetProcAddress(hNtdll, "RtlInitUnicodeString");
    RtlInitUnicodeString(&ustrModule, lpFileName);
    pLdrLoadDll _LdrLoadDll = (pLdrLoadDll)_GetProcAddress(hNtdll, "LdrLoadDll");
    if (!_LdrLoadDll) {
        return NULL;
    }
    NTSTATUS status = _LdrLoadDll(NULL, 0, &ustrModule, &hModule);
    return (HMODULE)hModule;
}