#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(void) {
  PU_SESSION session;
  pu_initialize(&session);
  ENetHost* host = pu_host_create(&session);
  while(true) {
    pu_update_network(host, &session);
  }
  pu_host_destroy(host);
  pu_deinitialize();
  return 0;
}
