#ifndef KV_STORE_H
#define KV_STORE_H

#define TABLE_SIZE 256

typedef struct
{
  char key[256];
  char value[256];
  int state; // 0 EMPTY, 1 OCCUPIED, 2 DELETED
} kv_pair;

typedef struct
{
  kv_pair table[TABLE_SIZE];
} kv_store;

void init_store(kv_store *store);

void set_key(kv_store *store, const char *key, const char *value);
int get_value(kv_store *store, const char *key, char *out);
int delete_key(kv_store *store, const char *key);

unsigned int hash(const char *key);

#endif