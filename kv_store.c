#include <string.h>
#include <stdio.h>
#include "kv_store.h"

void init_store(kv_store *store)
{
  store->count = 0;
}

void set_key(kv_store *store, const char *key, const char *value)
{
  for (int i = 0; i < store->count; i++)
  {
    if (strcmp(store->items[i].key, key) == 0)
    {
      snprintf(store->items[i].value, 256, "%s", value);
      return;
    }
  }

  if (store->count < MAX_KV)
  {
    snprintf(store->items[store->count].key, 256, "%s", key);
    snprintf(store->items[store->count].value, 256, "%s", value);
    store->count++;
  }
}

int get_value(kv_store *store, const char *key, char *out_value)
{
  for (int i = 0; i < store->count; i++)
  {
    if (strcmp(store->items[i].key, key) == 0)
    {
      snprintf(out_value, 256, "%s", store->items[i].value);
      return 1;
    }
  }
  return 0;
}

int delete_key(kv_store *store, const char *key)
{
  for (int i = 0; i < store->count; i++)
  {
    if (strcmp(store->items[i].key, key) == 0)
    {
      for (int j = i; j < store->count - 1; j++)
      {
        store->items[j] = store->items[j + 1];
      }
      store->count--;
      return 1;
    }
  }
  return 0;
}