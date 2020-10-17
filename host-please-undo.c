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
  ENetHost* host;
  int dt = 16667;

  pu_initialize(&session);
  host = pu_create_host(&session);

  do {
    pu_update_network(&session, host);
    if (session.has_started){
      pu_determine_sync_frame(&session);
      if (pu_rollback_condition(&session)) {
        pu_log("Should Rollback!\n");
        //printf("CURRENT FRAME: %d - REMOTE FRAME: %d - SYNC FRAME: %d\n", session.local_frame, session.remote_frame, session.sync_frame);
      }
      if (pu_timesynced_condition(&session)){
        session.local_frame++;
        uint16_t test_input = 22 + session.local_frame;
        pu_add_local_input(&session, host, test_input);
        pu_predict_remote_input(&session);
      }
    }
    usleep(dt);
  } while(true);

  pu_destroy_host(host, &session);
  pu_deinitialize(&session);
  return 0;
}
