// #include "includes.h"
#include <windows.h>
#include <stdio.h>
#include <gmp.h>

/*
Currently unimplemented:

Would implement Diffie-Hellman type protocol upon initialisation

C - win client
S - linux server

C -> S | Sends SIG_INIT
S -> C | Sends SIG_INIT
C -> S | Sends K_pub_c
S -> C | Sends K_pub_s
C -> S | SIG_HRBT

K_pub_c would be generated with a usage of
C: drive serial number and system info




D.1.2.3 Curve P-256
p = 1157920892103562487626974469494075735300861434152903141955
33631308867097853951
n = 115792089210356248762697446949407573529996955224135760342
422259061068512044369
SEED = c49d3608 86e70493 6a6678e1 139d26b7 819f7e90
c = 7efba166 2985be94 03cb055c 75d4f7e0 ce8d84a9 c5114abc
af317768 0104fa0d
b = 5ac635d8 aa3a93e7 b3ebbd55 769886bc 651d06b0 cc53b0f6
3bce3c3e 27d2604b
G x = 6b17d1f2 e12c4247 f8bce6e5 63a440f2 77037d81 2deb33a0
f4a13945 d898c296
G y = 4fe342e2 fe1a7f9b 8ee7eb4a 7c0f9e16 2bce3357 6b315ece
cbb64068 37bf51f5

OR
home aes


*/


int main(void){
    DWORD volumeSerialNumber;
    if (GetVolumeInformationW(L"C:\\", NULL, 0, &volumeSerialNumber, NULL, NULL, NULL, 0)) {
        wprintf(L"Volume Serial Number: %lu\n", volumeSerialNumber);
    }
    WORD P_arch;
    // Get the CPU information
    SYSTEM_INFO systemInfo;
    GetSystemInfo(&systemInfo);
    P_arch = systemInfo.wProcessorArchitecture;
    wprintf(L"Processor Architecture: %u\n", systemInfo.wProcessorArchitecture);

    mpz_t a, res;
    mpz_init(a);
    mpz_init(res);

    mpz_import(a, 1, 1, sizeof(DWORD), 0, 0, (void*)&volumeSerialNumber);
    mpz_pow_ui(res, a, *(unsigned int*)&P_arch);
    // char* str = mpz_get_str(NULL, 16, res);
    // printf("Value of myInteger: %s\n", str);
    return 0;
}