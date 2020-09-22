#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(int argc, char const *argv[]) {
  PU_SESSION session;
  PU_PLAYER_TYPE player_type = PLAYER_CLIENT;

  pu_initialize(&session);
  ENetHost* client = pu_client_create(&session);

  pu_client_destroy(client);
  pu_deinitialize();

  return 0;
}
