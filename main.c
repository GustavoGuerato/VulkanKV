#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

      char *cmd = strtok(buffer, " ");
      if (cmd == NULL)
      {
        continue;
      }

      char *key = strtok(NULL, " ");
      char *value = strtok(NULL, " ");

      if (strcmp(cmd, "SET") == 0)
      {
        if (key == NULL || value == NULL)
        {
          send(client_sd, "ERROR\n", 6, 0);
          continue;
        }

        send(client_sd, "OK\n", 3, 0);
      }
      else if (strcmp(cmd, "PING") == 0)
      {
        send(client_sd, "PONG\n", 5, 0);
      }
      else if (strcmp(cmd, "GET") == 0)
      {
        if (key == NULL)
        {
          send(client_sd, "ERROR\n", 6, 0);
          continue;
        }

        send(client_sd, "NOT FOUND\n", 10, 0);
      }
      else
      {
        send(client_sd, "UNKNOWN COMMAND\n", 16, 0);
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