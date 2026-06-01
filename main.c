#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define MAX_KV 100

typedef struct
{
  char key[256];
  char value[256];
} kv_pair;

void send_response(int client_sd, const char *response);
void send_error(int client_sd, const char *error_message);

void set_key(kv_pair *store, int *count, const char *key, const char *value);
int get_value(kv_pair *store, int count, const char *key, char *out_value);
int delete_key(kv_pair *store, int *count, const char *key);

int main()
{
  int server_sd, client_sd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);

  char buffer[1024];

  kv_pair kv_store[MAX_KV];
  int kv_count = 0;

  server_sd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_sd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(8080);

  if (bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_sd, 10) < 0)
  {
    perror("listen");
    close(server_sd);
    exit(EXIT_FAILURE);
  }

  printf("Server running on port 8080\n");

  while (1)
  {
    client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len);
    if (client_sd < 0)
    {
      perror("accept");
      continue;
    }

    printf("client connected\n");

    ssize_t bytes;
    while (1)
    {
      bytes = recv(client_sd, buffer, sizeof(buffer) - 1, 0);
      if (bytes <= 0)
        break;

      buffer[bytes] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0';

      char *cmd = strtok(buffer, " ");
      char *key = strtok(NULL, " ");
      char *value = strtok(NULL, " ");

      if (!cmd)
        continue;

      for (int i = 0; cmd[i]; i++)
        cmd[i] = toupper(cmd[i]);

      if (strcmp(cmd, "SET") == 0)
      {
        if (!key || !value)
        {
          send_error(client_sd, "MISSING ARGUMENTS");
          continue;
        }

        set_key(kv_store, &kv_count, key, value);
        send_response(client_sd, "OK\n");
      }

      else if (strcmp(cmd, "GET") == 0)
      {
        if (!key)
        {
          send_error(client_sd, "MISSING KEY");
          continue;
        }

        char value_out[256];
        if (get_value(kv_store, kv_count, key, value_out))
        {
          send_response(client_sd, value_out);
          send_response(client_sd, "\n");
        }
        else
        {
          send_error(client_sd, "NOT FOUND");
        }
      }

      else if (strcmp(cmd, "DEL") == 0)
      {
        if (!key)
        {
          send_error(client_sd, "MISSING KEY");
          continue;
        }

        if (delete_key(kv_store, &kv_count, key))
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

      printf("Received command: %s\n", cmd);
    }

    if (bytes < 0)
      perror("recv");

    close(client_sd);
    printf("client disconnected\n");
  }

  close(server_sd);
  return 0;
}

void set_key(kv_pair *store, int *count, const char *key, const char *value)
{
  for (int i = 0; i < *count; i++)
  {
    if (strcmp(store[i].key, key) == 0)
    {
      strcpy(store[i].value, value);
      return;
    }
  }

  if (*count < MAX_KV)
  {
    strcpy(store[*count].key, key);
    strcpy(store[*count].value, value);
    (*count)++;
  }
}

int get_value(kv_pair *store, int count, const char *key, char *out_value)
{
  for (int i = 0; i < count; i++)
  {
    if (strcmp(store[i].key, key) == 0)
    {
      strcpy(out_value, store[i].value);
      return 1;
    }
  }
  return 0;
}

int delete_key(kv_pair *store, int *count, const char *key)
{
  for (int i = 0; i < *count; i++)
  {
    if (strcmp(store[i].key, key) == 0)
    {
      for (int j = i; j < *count - 1; j++)
      {
        store[j] = store[j + 1];
      }
      (*count)--;
      return 1;
    }
  }
  return 0;
}

void send_response(int client_sd, const char *response)
{
  send(client_sd, response, strlen(response), 0);
  printf("Sent response: %s", response);
}

void send_error(int client_sd, const char *error_message)
{
  char buffer[512];
  snprintf(buffer, sizeof(buffer), "ERROR: %s\n", error_message);
  send_response(client_sd, buffer);
}