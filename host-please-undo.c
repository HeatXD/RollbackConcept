#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

int main(void) {
  PU_SESSION session;
  PU_SESSION_CALLBACKS cb;
  pu_initialize(&session);
  int dt = 16667;
  ENetHost* host = pu_create_host(&session);
  do {
    pu_run(&session, &cb, host);
    usleep(dt);
  } while(true);
  pu_destroy_host(host, &session);
  return 0;
}
