#ifndef _WEB_INCLUDES_
#define _WEB_INCLUDES_

#include "web_serv_lib/mongoose.h"

struct web_arg{
    struct db* db;
    int* termination;
};

void* web_main(void* _wa);

#endif