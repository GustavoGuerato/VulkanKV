#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
  int server_sd;
  int client_sd;
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  memset(&client_addr, 0, client_addr_len);
  char buffer[1024];
  struct kv_pair
  {
    char key[256];
    char value[256];
  };

  struct kv_pair kv_store[100];
  int kv_count = 0;
  if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

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
    return 1;
  }
  printf("Server is listening on port 8080\n");
  while (1)
  {
    client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sd < 0)
    {
      perror("accept");
      close(server_sd);
      return 1;
    }
    ssize_t bytes_read;
    printf("client connected\n");
    while ((bytes_read = recv(client_sd,
                              buffer,
                              sizeof(buffer) - 1,
                              0)) > 0)
    {
      buffer[bytes_read] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0';

      char *cmd = strtok(buffer, " ");
      if (cmd == NULL)
      {
        continue;
      }
      for (int i = 0; cmd[i] != '\0'; i++)
      {
        cmd[i] = toupper(cmd[i]);
      }
      char *key = strtok(NULL, " ");
      char *value = strtok(NULL, " ");

      if (strcmp(cmd, "SET") == 0)
      {
        if (key == NULL || value == NULL)
        {
          send(client_sd, "ERROR\n", strlen("ERROR\n"), 0);
          continue;
        }

        int found = 0;

        for (int i = 0; i < kv_count; i++)
        {
          if (strcmp(kv_store[i].key, key) == 0)
          {
            strcpy(kv_store[i].value, value);
            found = 1;
            break;
          }
        }

        if (!found)
        {
          if (kv_count >= 100)
          {
            send_error(client_sd, "Store full\n");
            continue;
          }

          strcpy(kv_store[kv_count].key, key);
          strcpy(kv_store[kv_count].value, value);

          kv_count++;
        }

        send_response(client_sd, "OK\n");
      }

      else if (strcmp(cmd, "PING") == 0)
      {
        send_response(client_sd, "PONG\n");
      }

      else if (strcmp(cmd, "GET") == 0)
      {
        if (key == NULL)
        {
          send_error(client_sd, "MISSING KEY\n");
          continue;
        }

        int found = 0;

        for (int i = 0; i < kv_count; i++)
        {
          if (strcmp(kv_store[i].key, key) == 0)
          {
            char response[512];

            snprintf(response,
                     sizeof(response),
                     "%s\n",
                     kv_store[i].value);

            send(client_sd,
                 response,
                 strlen(response),
                 0);

            found = 1;
            break;
          }
        }

        if (!found)
        {
          send_error(client_sd, "NOT FOUND\n");
        }
      }
      else if (strcmp(cmd, "DEL") == 0)
      {
        int found = 0;

        for (int i = 0; i < kv_count; i++)
        {
          if (strcmp(kv_store[i].key, key) == 0)
          {
            for (int j = i; j < kv_count - 1; j++)
            {
              kv_store[j] = kv_store[j + 1];
            }
            kv_count--;
            found = 1;
            break;
          }
        }

        if (found)
        {
          send_response(client_sd, "OK\n");
        }
        else
        {
          send_error(client_sd, "NOT FOUND\n");
        }
      }
      else
      {
        send_error(client_sd, "UNKNOWN COMMAND\n");
      }

      printf("Received message: %s\n", cmd);
    }
    if (bytes_read < 0)
    {
      perror("recv");
    }
    else if (bytes_read == 0)
    {
      printf("client disconnected\n");
    }

    close(client_sd);
  }

  close(server_sd);
  return 0;
}

void send_response(int client_sd, const char *response)
{
  if (send(client_sd, response, strlen(response), 0) < 0)
  {
    perror("send");
  }
  else
  {
    printf("Sent response: %s", response);
  }
}
void send_error(int client_sd, const char *error_message)
{
  char response[512];
  snprintf(response, sizeof(response), "ERROR: %s\n", error_message);
  send_response(client_sd, response);
}