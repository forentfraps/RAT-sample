#include "db.h"
#include "utils.h"
#include "netlib.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

// struct dcell{
//     char ip[16];
//     struct sockaddr_in sa;
//     unsigned long long hash;
//     long timestamp;
// };

// struct db{
//     int len;
//     struct dcell** st;

// };

unsigned long long hash(const char str[16])
{
    unsigned long long hashValue = 0;
    for (int i = 0; i < 16; i++) {
        hashValue = ((hashValue << 5) + hashValue) + str[i];
    }
    
    return hashValue;
}

int create_db(struct db** db)
{
    struct dcell* cell = malloc(sizeof(struct dcell));
    struct dcell** st = malloc(sizeof(struct dcell*));
    *db = malloc(sizeof(struct db));
    if (cell && st && *db) {
        memset(cell, 0, sizeof(*cell));
        st[0] = cell;
        (*db)->st = st;
        (*db)->len = 1;
        return 0;
    } else {
        DBGLG("Failed to malloc\n");
        if (cell) free(cell);
        if (st) free(st);
        if (*db) free(*db);
        *db = NULL;
        return -1;
    }
}
int add_db(struct db* db, char* sa, long timestamp)
{
    struct dcell* cell;
    struct dcell* ptr;
    char* tmp = get_ip(*(struct sockaddr_in*)sa);    
    unsigned long long h = hash(tmp);
    for( int i = 0; i < db->len - 1; ++i){
        if(db->st[i]->hash == h){
            DBGLG("sockaddr already in db, updating sa\n");
            memcpy(&(db->st[i]->sa), sa, sizeof(struct sockaddr_in));
            return 1;
        }
    }
    ptr = db->st[db->len - 1];
    if (ptr == NULL){
        DBGLG("Null in db cells\n");
        return -1;
    }
    memcpy(&(ptr->ip), tmp, 16);
    memcpy(&(ptr->sa), sa, sizeof(struct sockaddr_in));
    ptr->hash = h;
    ptr->timestamp = timestamp;
    if ((cell = malloc(sizeof(struct dcell))) == NULL){
        DBGLG("Failed to malloc in add_db\n");
        return -1;
    }
    memset(cell, 0, sizeof(struct dcell));
    if((db->st = realloc(db->st, (sizeof(cell)) * (db->len + 1))) == NULL){
        DBGLG("DB DOWN, failed to realloc :)\n");
        return -1;
    }
    db->st[db->len++] = cell;
    return 0;
}

void print_db(struct db* db)
{
    printf("List of ips:\n");
    for(int i = 0; i < db->len - 1; ++i){
        printf("%d. %s %lu\n", i, db->st[i]->ip, db->st[i]->timestamp);
        // printf("%llu\n", *(unsigned long long*)(db->st[i]->ip));
        // printf("%llu\n", *(unsigned long long*)(db->st[i]->ip + 8));
        // printf("%llu\n", *(unsigned long long*)(db->st[i]->sa));
        // printf("%llu\n", *(unsigned long long*)(db->st[i]->sa + 8));
        // printf("%llu\n", (unsigned long long)(db->st[i]->hash));
        // printf("%ld\n", (db->st[i]->timestamp));
    }
    printf("\n");
}

int upd_timestamp_db(struct db* db, char* ip)
{
    int i =0;
    unsigned long long h = hash(ip);
    for (; i < db->len - 1; ++i){
        if (db->st[i]->hash == h){
            goto upd_timestamp_db_success;
        }
    }
    return -1;
    upd_timestamp_db_success:
    db->st[i]->timestamp = time(NULL);
    return 0;
}

int load_db(struct db** db, char* path)
{
    FILE* fd;
    fd = fopen(path, "r");
    if (fd == NULL){
        DBGLG("Invalid path for db loading\n");
        return -1;
    }
    int len;
    char tmp[sizeof(struct dcell)];

    fread(&len, sizeof(int), 1, fd);
    DBGLG("Length is : ", len);
    create_db(db);
    for (int i = 0; i < len -1; ++i){
        DBGLG("Loading a cell\n");
        if (fread((*db)->st[i], sizeof(struct dcell), 1, fd) > 0){
            (*db)->st = realloc((*db)->st, sizeof(struct dcell*) * (++(*db)->len));
            (*db)->st[i+1] = malloc(sizeof(struct dcell));
            memset((*db)->st[i+1], 0, sizeof(struct dcell));
        }
    }
    DBGLG("Len is: ", (*db)->len);
    fclose(fd);
    return 0;
}

/*
TODO Add a service that purges the db if heartbeat timestamps are too old
*/

int save_db(struct db* db, char* path)
{
    FILE* fd;
    struct dcell* ptr;
    
    if ((fd = fopen(path, "w")) == NULL) {
        DBGLG("Failed to open path\n");
        return -1;
    }
    
    fwrite(&(db->len), sizeof(int), 1, fd);

    for (int i = 0; i < db->len - 1; ++i) {
        DBGLG("Saving cell number: ", i);
        ptr = db->st[i];
        fwrite(ptr, sizeof(struct dcell), 1, fd);
        DBGLG("End saving cell\n");
    }
    
    fclose(fd);
    return 0;
}


void free_db(struct db** db)
{
    DBGLG("Len is : ", (*db)->len);
    for (int i = 0; i < (*db)->len; ++i){
        free((*db)->st[i]);
    }
    free((*db)->st);
    free(*db);
}

