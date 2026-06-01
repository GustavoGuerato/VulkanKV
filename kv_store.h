#ifndef KV_STORE_H
#define KV_STORE_H

#define MAX_KV 100

typedef struct
{
  char key[256];
  char value[256];
} kv_pair;

typedef struct
{
  kv_pair items[MAX_KV];
  int count;
} kv_store;

void init_store(kv_store *store);

void set_key(kv_store *store, const char *key, const char *value);
int get_value(kv_store *store, const char *key, char *out_value);
int delete_key(kv_store *store, const char *key);

#endif