#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(void) {
  PU_SESSION session;
  pu_initialize(&session);
  ENetHost* client = pu_client_create(&session);
  int result = 1;
  if (!pu_connect_to_host(client, &session, "127.0.0.1")) {
    result = 0;
  }
  while (result == 1) {
    pu_update_network(client, &session);
  }
  pu_disconnect_from_host(&session, client);
  pu_client_destroy(client);
  pu_deinitialize();
  return 0;
}
