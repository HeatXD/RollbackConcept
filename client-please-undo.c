#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include <stdio.h>

void save_state(int frame){
  printf("Save Game State at Frame: %d\n",frame);
}

void restore_state(int frame) {
  printf("Restore Game State to Frame: %d\n",frame);
}

int main(void) {
  PU_SESSION session;
  ENetHost* client;
  int dt = 16667;

  pu_initialize(&session);
  client = pu_create_client(&session);
  int result = pu_connect_to_host(client, &session, "127.0.0.1");

  do {
    pu_update_network(&session, client);
    if (session.has_started){
      pu_determine_sync_frame(&session);
      if (pu_rollback_condition(&session)) {
        pu_log("Should Rollback!\n");

        //printf("CURRENT FRAME: %d - REMOTE FRAME: %d - SYNC FRAME: %d\n", session.local_frame, session.remote_frame, session.sync_frame);
      }
      if (pu_timesynced_condition(&session)){
        pu_log("normal update\n");
        session.local_frame++;
        uint16_t test_input = 33 + session.local_frame;
        //pu_send_input(&session, client, test_input);
        pu_add_local_input(&session, client, test_input);
        pu_predict_remote_input(&session);
        //advance gamestate
        //save gamestate
      }
    }
    usleep(dt);
  } while(result == 1);

  pu_disconnect_from_host(&session, client);
  pu_destroy_client(client);
  pu_deinitialize(&session);
  return 0;
}
