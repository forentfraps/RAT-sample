#include "db.h"
#include "utils.h"
#include "netlib.h"

#include <string.h>
#include <stdlib.h>

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
    struct dcell** st = malloc(sizeof(cell));
    *db = malloc(sizeof(struct db));
    if(cell && st && *db == NULL){
        DBGLG("Failed to malloc\n");
        *db = NULL;
        return -1;
    }
    memset(cell, 0, sizeof(*cell));
    st[0] = cell;
    (*db)->st = st;
    (*db)->len = 1;
    return 0;
}

int add_db(struct db* db, char* sa, long timestamp)
{
    struct dcell* cell;
    struct dcell* ptr;
    char* tmp = get_ip(*(struct sockaddr_in*)sa);    
    unsigned long long h = hash(tmp);
    for( int i = 0; i < db->len - 1; ++i){
        if(db->st[i]->hash == h){
            DBGLG("sockaddr already in db\n");
            return 1;
        }
    }
    ptr = db->st[db->len - 1];
    if (!ptr){
        DBGLG("Null in db cells\n");
        return -1;
    }
    memcpy(&(ptr->ip), tmp, 16);
    memcpy(&(ptr->sa), sa, 16);
    ptr->hash = h;
    ptr->timestamp = timestamp;
    if ((cell = malloc(sizeof(struct dcell))) == NULL){
        DBGLG("Failed to malloc in add_db\n");
        return -1;
    }
    memset(cell, 0, 24);
    if((db->st = realloc(db->st, (sizeof(cell)) * (db->len + 1))) == NULL){
        DBGLG("DB DOWN, failed to realloc :)\n");
        return -1;
    }
    db->st[db->len] = cell;
    db->len = db->len + 1;
    return 0;
}

/*
TODO Add a service that purges the db if heartbeat timestamps are too old
*/

int save_db(struct db* db, char* path)
{
    FILE* fd;
    struct dcell* ptr;
    if ((fd = fopen(path, "w")) == NULL){
        DBGLG("Failed to open path\n");
        return -1;
    }
    fwrite(&(db->len), sizeof(int), 1, fd);

    for (int i = 0; i < db->len; ++i){
        ptr = db->st[i];
        fwrite(ptr, sizeof(*ptr), 1, fd);
    }
    fclose(fd);
    return 0;

}

void free_db(struct db** db)
{
    for (int i = 0; i < (*db)->len; ++i){
        free((*db)->st[i]);
    }
    free((*db)->st);
    free(*db);
}

