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
}