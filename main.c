#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
int main(void)
{
  int server_sd;
  int client_sd;
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  memset(&client_addr, 0, client_addr_len);
  char buffer[1024];

  if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    printf("Error to create socket\n");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(8080);

  if (bind(server_sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
  {
    printf("Error to bind socket\n");
    exit(EXIT_FAILURE);
  }

  if (listen(server_sd, 10) < 0)
  {
    perror("listen");
    close(server_sd);
    return 1;
  }
  printf("Server is listening on port 8080\n");

  client_sd = accept(server_sd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_sd < 0)
  {
    perror("accept");
    close(server_sd);
    return 1;
  }

  printf("client connected\n");

  ssize_t bytes_read = recv(client_sd, buffer, sizeof(buffer) - 1, 0);
  if (bytes_read < 0)
  {
    perror("recv");
  }
  else if (bytes_read == 0)
  {
    printf("client disconnected\n");
  }
  else
  {
    buffer[bytes_read] = '\0';
    printf("Received message: %s\n", buffer);
  }

  recv(client_sd, buffer, sizeof(buffer) - 1, 0);
  close(server_sd);
  close(client_sd);
  return 0;
}