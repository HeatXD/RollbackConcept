#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

void save_state(int frame){
  printf("Save Game State at Frame: %d\n",frame);
}

void restore_state(int frame) {
  printf("Restore Game State at Frame: %d\n",frame);
}

int main(void) {
  PU_SESSION session;
  PU_SESSION_CALLBACKS cb;
  ENetHost* host;
  int dt = 16667;

  cb.restore_game_state = restore_state;
  cb.save_game_state = save_state;

  pu_initialize(&session);
  host = pu_create_host(&session);

  do {
    pu_run(&session, &cb, host);
    usleep(dt);
  } while(true);

  pu_destroy_host(host, &session);
  return 0;
}
