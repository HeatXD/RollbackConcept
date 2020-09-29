#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(void) {
  PU_SESSION session;
  PU_SESSION_CALLBACKS cb;
  ENetHost* client;
  int dt = 16667;
  pu_initialize(&session);
  client = pu_create_client(&session);
  int result = pu_connect_to_host(client, &session, "127.0.0.1");
  do {
    pu_run(&session, &cb, client);
    usleep(dt);
  } while(result == 1);
  pu_disconnect_from_host(&session, client);
  pu_destroy_client(client);
  return 0;
}
