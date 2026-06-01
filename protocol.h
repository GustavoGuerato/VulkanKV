#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "kv_store.h"

void handle_command(kv_store *store, int client_sd, char *buffer);

#endif