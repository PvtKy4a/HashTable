#include "HashTable.h"
#include <stdlib.h>
#include <string.h>

typedef struct Node {
  char * key;
  void * data;
  struct Node * next;
  struct Node * prev;
} Node_t;

typedef struct {
  Node_t ** table;
  uint16_t capacity;
  ht_return_status_t init_status;
} HashTable_t;

HashTable_t hash_table = {0};

ht_return_status_t ht_init_hash_table(uint16_t capacity) {
  hash_table.table = (Node_t **)malloc(capacity * sizeof(hash_table.table));
  if (hash_table.table == NULL) {
    hash_table.init_status = HT_INIT_FAILED;
    return HT_INIT_FAILED;
  }
  for (uint8_t i = 0; i < capacity; i++) {
    hash_table.table[i] = NULL;
  }
  hash_table.capacity = capacity;
  hash_table.init_status = HT_INIT_SUCCEEDED;
  return HT_INIT_SUCCEEDED;
}

Node_t * create_node(const char * key, void * data) {
  uint8_t key_size = strlen(key);
  Node_t * new_node = (Node_t *)malloc(sizeof(*new_node));
  if (new_node == NULL) {
    return NULL;
  }
  new_node->key = (char *)malloc(sizeof(*key) * (key_size + 1));
  if (new_node->key == NULL) {
    free(new_node);
    return NULL;
  }
  new_node->key = strcpy(new_node->key, key);
  new_node->key[key_size] = '\0';
  new_node->data = data;
  new_node->next = NULL;
  new_node->prev = NULL;
  return new_node;
}

void delete_node(Node_t * node) {
  free(node->key);
  free(node);
}

uint16_t get_index(const char * key) {
  int16_t hash = 0;
  while(*key != '\0') {
    hash = (hash << 5) - hash + *key;
    key++;
  }
  hash = abs(hash);
  return (uint16_t)hash % hash_table.capacity;
}

ht_return_status_t ht_add_or_update(const char * key, void * data) {
  if (hash_table.init_status == HT_INIT_FAILED) {
    return HT_INIT_FAILED;
  }
  uint16_t index = get_index(key);
  Node_t * new_node = create_node(key, data);
  Node_t * node = NULL;
  if (new_node == NULL) {
    return HT_MALLOC_FAILED;
  }
  if (hash_table.table[index] == NULL) {
    hash_table.table[index] = new_node;
    return HT_DATA_ADDED;
  }
  if (strcmp(hash_table.table[index]->key, key) == 0) {
    hash_table.table[index]->data = data;
    delete_node(new_node);
    return HT_DATA_UPDATED;
  }
  node = hash_table.table[index];
  while (node->next != NULL) {
    node = node->next;
    if (strcmp(node->key, key) == 0) {
      node->data = data;
      delete_node(new_node);
      return HT_DATA_UPDATED;
    }
  }
  new_node->prev = node;
  node->next = new_node;
  return HT_DATA_ADDED;
}

Node_t * get_node(const char * key, uint16_t index) {
  Node_t * node = hash_table.table[index];
  while (node != NULL) {
    if (strcmp(node->key, key) == 0) {
      return node;
    }
  }
  return NULL;
}

void * ht_pop(const char * key) {
  if (hash_table.init_status == HT_INIT_FAILED) {
    return NULL;
  }
  uint16_t index = get_index(key);
  Node_t * node = get_node(key, index);
  Node_t * temp_node = NULL;
  void * data = NULL;
  if (node == NULL) {
    return NULL;
  }
  data = node->data;
  if (node == hash_table.table[index] && node->next == NULL) {
    delete_node(node);
    hash_table.table[index] = NULL;
    return data;
  }
  if (node == hash_table.table[index] && node->next != NULL) {
    hash_table.table[index] = node->next;
    hash_table.table[index]->prev = NULL;
    delete_node(node);
    return data;
  }
  if (node->prev != NULL && node->next == NULL) {
    temp_node = node->prev;
    temp_node->next = NULL;
    delete_node(node);
    return data;
  }
  temp_node = node->prev;
  temp_node->next = node->next;
  temp_node = node->next;
  temp_node->prev = node->prev;
  delete_node(node);
  return data;
}

ht_return_status_t ht_delete(const char * key) {
  if (hash_table.init_status == HT_INIT_FAILED) {
    return HT_INIT_FAILED;
  }
  void * data = ht_pop(key);
  if (data == NULL) {
    return HT_KEY_NOT_FOUND;
  }
  return HT_DATA_DELETED;
}

void * ht_get(const char * key) {
  if (hash_table.init_status == HT_INIT_FAILED) {
    return NULL;
  }
  uint16_t index = get_index(key);
  Node_t * node = get_node(key, index);
  if (node == NULL) {
    return NULL;
  }
  return node->data;
}
