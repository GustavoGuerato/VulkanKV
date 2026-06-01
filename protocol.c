#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h>

#include "protocol.h"

void send_response(int client_sd, const char *response)
{
  send(client_sd, response, strlen(response), 0);
}

void send_error(int client_sd, const char *msg)
{
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "ERROR: %s\n", msg);
  send_response(client_sd, buffer);
}

void handle_command(kv_store *store, int client_sd, char *buffer)
{
  char *cmd = strtok(buffer, " ");
  char *key = strtok(NULL, " ");
  char *value = strtok(NULL, " ");

  if (!cmd)
    return;

  for (int i = 0; cmd[i]; i++)
    cmd[i] = toupper(cmd[i]);

  if (strcmp(cmd, "SET") == 0)
  {
    if (!key || !value)
      return send_error(client_sd, "MISSING ARGUMENTS");

    set_key(store, key, value);
    send_response(client_sd, "OK\n");
  }
  else if (strcmp(cmd, "GET") == 0)
  {
    char out[256];

    if (get_value(store, key, out))
    {
      send_response(client_sd, out);
      send_response(client_sd, "\n");
    }
    else
    {
      send_error(client_sd, "NOT FOUND");
    }
  }
  else if (strcmp(cmd, "DEL") == 0)
  {
    if (delete_key(store, key))
      send_response(client_sd, "OK\n");
    else
      send_error(client_sd, "NOT FOUND");
  }
  else if (strcmp(cmd, "PING") == 0)
  {
    send_response(client_sd, "PONG\n");
  }
  else
  {
    send_error(client_sd, "UNKNOWN COMMAND");
  }
}