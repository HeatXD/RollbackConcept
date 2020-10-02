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
  ENetHost* client;
  int dt = 16667;

  cb.restore_game_state = restore_state;
  cb.save_game_state = save_state;

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
