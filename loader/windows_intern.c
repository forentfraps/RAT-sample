#include "loader.h"


extern unsigned char ntdll_full_path[];
extern unsigned char cLdrLoadDll[];
extern unsigned char cRtlInitUnicodeString[];

wchar_t FORCE_INLINE *custom_wcsncpy(wchar_t* dest, const wchar_t* src, size_t count) {
    // DYNAMIC_RETURN;
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

unsigned int FORCE_INLINE crc32s(const wchar_t* message) {
    // DYNAMIC_RETURN;
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (message[i] != L'\0') {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

size_t  FORCE_INLINE custom_wcslen(const wchar_t* str) {
    // DYNAMIC_RETURN;
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
int  FORCE_INLINE custom_wcscmp(const wchar_t* str1, const wchar_t* str2) {
    // while (*str1 == *str2 && *str1 != L'\0') {
    //     str1++;
    //     str2++;
    // }
    // return *str1 - *str2;
    // DYNAMIC_RETURN;
    return crc32s(str1) != crc32s(str2);
}
errno_t  FORCE_INLINE custom_wcscpy_s(wchar_t* dest, size_t destSize, const wchar_t* src) {
    // DYNAMIC_RETURN;
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
int FORCE_INLINE custom_strcmp(const char* str1, const char* str2) {
    // DYNAMIC_RETURN;
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
FARPROC FORCE_INLINE _GetProcAddress(HMODULE hModule, LPCSTR lpProcName) {
    // DYNAMIC_RETURN;
    int x = 0;
    PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)hModule;
    x = (int)dosHeader * 0x123881;
    PIMAGE_NT_HEADERS ntHeaders = (PIMAGE_NT_HEADERS)((BYTE*)hModule + dosHeader->e_lfanew);
    x -= (int)ntHeaders / 0x12;
    PIMAGE_EXPORT_DIRECTORY exportDirectory = (PIMAGE_EXPORT_DIRECTORY)((BYTE*)hModule +
        ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    x += (int) exportDirectory - 0x41223;
    DWORD* addressOfFunctions = (DWORD*)((BYTE*)hModule + exportDirectory->AddressOfFunctions);
    WORD* addressOfNameOrdinals = (WORD*)((BYTE*)hModule + exportDirectory->AddressOfNameOrdinals);
    DWORD* addressOfNames = (DWORD*)((BYTE*)hModule + exportDirectory->AddressOfNames);
    for (DWORD i = 0; i < exportDirectory->NumberOfNames; ++i) {
        #ifndef DEBUG
        TOMFOOLERY;
        #endif
        if (custom_strcmp(lpProcName, (const char*)hModule + addressOfNames[i]) == 0) {
            return (FARPROC)((BYTE*)hModule + addressOfFunctions[addressOfNameOrdinals[i]]);
        }
    }
    return NULL;
}

HMODULE FORCE_INLINE _GetModuleHandle(LPCWSTR lModuleName) {
    // DYNAMIC_RETURN;
    PEB* pPeb = (PEB*)__readgsqword(0x60);
    int x = 0;
    // For x86
    // PEB* pPeb = (PEB*)__readgsqword(0x30);
    PEB_LDR_DATA* Ldr = pPeb->Ldr;
    x = (int)Ldr * 0x123881;
    LIST_ENTRY* ModuleList = &Ldr->InMemoryOrderModuleList;
    LIST_ENTRY* pStartListEntry = ModuleList->Flink;
    x -= (int)pStartListEntry / 0x12;
    WCHAR mystr[MAX_PATH] = { 0 };
    WCHAR substr[MAX_PATH] = { 0 };
    for (LIST_ENTRY* pListEntry = pStartListEntry; pListEntry != ModuleList; pListEntry = pListEntry->Flink) {
        LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)((BYTE*)pListEntry - sizeof(LIST_ENTRY));
        // custom_memset(mystr, 0, MAX_PATH * sizeof(WCHAR));
        // custom_memset(substr, 0, MAX_PATH * sizeof(WCHAR));
        x += (int) pEntry - 0x41223;
        // custom_wcscpy_s(mystr, MAX_PATH, pEntry->FullDllName.Buffer);
        // custom_wcscpy_s(substr, MAX_PATH, lModuleName);
        if (custom_wcscmp(pEntry->FullDllName.Buffer, lModuleName) == 0) {
            // Returning the DLL base address
            return (HMODULE)pEntry->DllBase;
        }
    }
}
HMODULE FORCE_INLINE _LoadLibrary(LPCWSTR lpFileName) {
    UNICODE_STRING ustrModule;
    int x;
    HANDLE hModule = NULL;
    HMODULE hNtdll = _GetModuleHandle(ntdll_full_path);
    // DYNAMIC_RETURN;
    x *= (int)hNtdll ^ x;
    pRtlInitUnicodeString RtlInitUnicodeString = (pRtlInitUnicodeString)_GetProcAddress(hNtdll, cRtlInitUnicodeString);
    RtlInitUnicodeString(&ustrModule, lpFileName);
    #ifndef DEBUG
    TOMFOOLERY
    #endif
    x |= (int)RtlInitUnicodeString;
    pLdrLoadDll _LdrLoadDll = (pLdrLoadDll)_GetProcAddress(hNtdll, cLdrLoadDll);
    if (!_LdrLoadDll) {
        return NULL;
    }
    NTSTATUS status = _LdrLoadDll(NULL, 0, &ustrModule, &hModule);
    return (HMODULE)hModule;
}