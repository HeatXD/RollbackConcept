#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(void) {
  PU_SESSION session;
  ENetHost* client;
  pu_initialize();
  client = pu_create_client(&session);
  int result = pu_connect_to_host(client, &session, "127.0.0.1");
  int count = 0;
  do {
    pu_update_network(&session, client);
    if (count > 10) {
      result = 0;
    }
    count++;
    sleep(1);
  } while(result == 1);
  pu_disconnect_from_host(&session, client);
  pu_destroy_client(client);
  return 0;
}
