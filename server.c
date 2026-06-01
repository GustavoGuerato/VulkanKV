#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "kv_store.h"
#include "protocol.h"

void start_server(int port)
{
  int server_sd, client_sd;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len = sizeof(client_addr);

  char buffer[1024];

  kv_store store;
  init_store(&store);

  server_sd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  listen(server_sd, 10);

  printf("Server running on port %d\n", port);

  while (1)
  {
    client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_len);
    printf("client connected\n");

    ssize_t bytes;

    while (1)
    {
      bytes = recv(client_sd, buffer, sizeof(buffer) - 1, 0);
      if (bytes <= 0)
        break;

      buffer[bytes] = '\0';
      buffer[strcspn(buffer, "\r\n")] = '\0';

      handle_command(&store, client_sd, buffer);
    }

    close(client_sd);
    printf("client disconnected\n");
  }

  close(server_sd);
}