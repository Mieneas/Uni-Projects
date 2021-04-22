#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "uthash.h"

typedef struct htable{
    unsigned char* key;
    size_t key_len;
    unsigned char* value;
    size_t value_len;
    UT_hash_handle hh;
} htable;

void htable_set(htable** ht, const unsigned char* key, size_t key_len, const unsigned char* value, size_t value_len);
htable* htable_get(htable** ht, const unsigned char* key, size_t key_len);
int htable_delete(htable** ht, const unsigned char* key, size_t key_len);

#endif