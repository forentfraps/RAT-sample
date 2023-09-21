#ifndef _DB_H_INCLUDED
#define _DB_H_INCLUDED


struct dcell{
    char ip[16];
    char sa[16];
    unsigned long long hash;
    // LEAVE TIMESTAMP LAST
    long timestamp;
};

struct db{
    int len;
    struct dcell** st;
};


int create_db(struct db** db);
int add_db(struct db* db, char* sa, long timestamp);
int save_db(struct db* db, char* path);
unsigned long long hash(const char str[16]);
void free_db(struct db** db);
void print_db(struct db* db);
int upd_timestamp_db(struct db* db, char* ip);
int load_db(struct db** db, char* path);
#endif