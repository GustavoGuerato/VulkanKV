#include "server.h"
#include "protocol.h"
#include "kv_store.h"

int main()
{
  start_server(8080);
  return 0;
}