#include <string.h>
#include <stdio.h>
#include "kv_store.h"

#define TABLE_SIZE 256

unsigned int hash(const char *key)
{
  unsigned int h = 0;
  while (*key)
    h = (h * 31) + (unsigned char)(*key++);
  return h % TABLE_SIZE;
}

void init_store(kv_store *store)
{
  for (int i = 0; i < TABLE_SIZE; i++)
  {
    store->table[i].state = 0;
  }
}

void set_key(kv_store *store, const char *key, const char *value)
{
  unsigned int index = hash(key);
  int first_deleted = -1;

  for (int i = 0; i < TABLE_SIZE; i++)
  {
    int idx = (index + i) % TABLE_SIZE;

    if (store->table[idx].state == 1)
    {
      if (strcmp(store->table[idx].key, key) == 0)
      {
        snprintf(store->table[idx].value, 256, "%s", value);
        return;
      }
    }
    else if (store->table[idx].state == 2 && first_deleted == -1)
    {
      first_deleted = idx;
    }
    else if (store->table[idx].state == 0)
    {
      break;
    }
  }

  int idx = (first_deleted != -1) ? first_deleted : index;

  store->table[idx].state = 1;
  snprintf(store->table[idx].key, 256, "%s", key);
  snprintf(store->table[idx].value, 256, "%s", value);
}

int get_value(kv_store *store, const char *key, char *out_value)
{
  unsigned int index = hash(key);

  for (int i = 0; i < TABLE_SIZE; i++)
  {
    int idx = (index + i) % TABLE_SIZE;

    if (store->table[idx].state == 0)
      return 0;

    if (store->table[idx].state == 1 &&
        strcmp(store->table[idx].key, key) == 0)
    {
      snprintf(out_value, 256, "%s", store->table[idx].value);
      return 1;
    }
  }

  return 0;
}

int delete_key(kv_store *store, const char *key)
{
  unsigned int index = hash(key);

  for (int i = 0; i < TABLE_SIZE; i++)
  {
    int idx = (index + i) % TABLE_SIZE;

    if (store->table[idx].state == 0)
      return 0;

    if (store->table[idx].state == 1 &&
        strcmp(store->table[idx].key, key) == 0)
    {
      store->table[idx].state = 2;
      return 1;
    }
  }

  return 0;
}