#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdint.h>

typedef enum {
  HT_DATA_ADDED,
  HT_DATA_UPDATED,
  HT_DATA_DELETED,
  HT_MALLOC_FAILED,
  HT_INIT_FAILED,
  HT_INIT_SUCCEEDED,
  HT_KEY_NOT_FOUND
} ht_return_status_t;

ht_return_status_t ht_init_hash_table(uint16_t capacity);

ht_return_status_t ht_add_or_update(const char * key, void * data);

void * ht_pop(const char * key);

ht_return_status_t ht_delete(const char * key);

void * ht_get(const char * key);

#endif
