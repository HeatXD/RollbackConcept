#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>
#include "C:/raylib/raylib/src/raylib.h"

int main(int argc, char const *argv[]) {
  PU_SESSION session;
  pu_initialize(&session);
  
  ENetHost* client = pu_client_create();
  pu_client_destroy(client);
  pu_deinitialize();
  return 0;
}
