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

        strcpy(kv_store[kv_count].key, key);
        strcpy(kv_store[kv_count].value, value);

        kv_count++;

        send(client_sd, "OK\n", strlen("OK\n"), 0);
      }

      else if (strcmp(cmd, "PING") == 0)
      {
        send(client_sd, "PONG\n", strlen("PONG\n"), 0);
      }

      else if (strcmp(cmd, "GET") == 0)
      {
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
          send(client_sd,
               "NOT FOUND\n",
               strlen("NOT FOUND\n"),
               0);
        }
        if (key == NULL)
        {
          send(client_sd, "ERROR\n", strlen("ERROR\n"), 0);
          continue;
        }

        send(client_sd, "NOT FOUND\n", strlen("NOT FOUND\n"), 0);
      }
      else
      {
        send(client_sd, "UNKNOWN COMMAND\n", strlen("UNKNOWN COMMAND\n"), 0);
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
