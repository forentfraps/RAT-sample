#include "bs_config.h"
#include "utils.h"
#include <stdio.h>

void DBGLG(char buf[], ...)
{
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

void isc(int* ptr)
{
    while(scanf("%d", ptr) != 1){
        printf("Bad input, reenter!\n");
        scanf("%*[^\n]");
    }
}