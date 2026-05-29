#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
int main(void)
{
  int server_sd;
  struct sockaddr_in server_addr;
  if ((server_sd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
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
  if (listen(server_sd, 3) < 0)
  {
    printf("Error to listen socket\n");
    exit(EXIT_FAILURE);
  }
  printf("Server is listening on port 8080\n");
  close(server_sd);
  return 0;
}