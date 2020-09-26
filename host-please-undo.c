#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(void) {
  PU_SESSION session;
  pu_initialize();
  ENetHost* host = pu_create_host(&session);
  while (true) {
    pu_update_network(&session, host);
  }
  pu_destroy_host(host, &session);
  return 0;
}
