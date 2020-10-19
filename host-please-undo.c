#define PLEASE_UNDO_IMPL_H
#include "include/please_undo.h"
#include "include/cvector.h"
#include <stdio.h>

typedef struct GameState{
  int Member1;
  uint16_t Member2, Member3;
}GameState;

typedef struct GameStateVector{
  GameState* gs_vector;
}GameStateVector;

GameState gs = {0,0,0};
GameStateVector gs_vec = {.gs_vector = NULL};

void save_game_state(int frame, GameStateVector* gs_vec, GameState* gs){
  //printf("//====================================================//\n");
//  printf("Save Game State at Frame: %d\n", frame);
  if (frame >= vector_capacity(gs_vec->gs_vector)) {
    vector_reserve(gs_vec->gs_vector,vector_capacity(gs_vec->gs_vector) + 30);
  }
  GameState temp_gs;
  temp_gs.Member1 = gs->Member1;
  temp_gs.Member2 = gs->Member2;
  temp_gs.Member3 = gs->Member3;

  printf("Saved state frame %d >> %d|%u|%u\n", frame, temp_gs.Member1, temp_gs.Member2, temp_gs.Member3);
  gs_vec->gs_vector[frame-1] = temp_gs;
}

void restore_game_state(int frame, GameStateVector* gs_vec, GameState* gs){
//  printf("//====================================================//\n");
//  printf("Restore Game State at Frame: %d\n", frame);
  printf("Restored state frame %d >> %d|%u|%u\n", frame, gs_vec->gs_vector[frame-1].Member1, gs_vec->gs_vector[frame-1].Member2, gs_vec->gs_vector[frame-1].Member3);
  gs->Member1 = gs_vec->gs_vector[frame-1].Member1;
  gs->Member2 = gs_vec->gs_vector[frame-1].Member2;
  gs->Member3 = gs_vec->gs_vector[frame-1].Member3;
}

void advance_game_state(int frame, GameState* gs, PU_INPUT_STORAGE* inputs){
//  printf("//====================================================//\n");
//  printf("Advance the Game State Forward one step\n");
  gs->Member1 += 2;
  gs->Member2 = inputs[0].input_vector[frame-1].input;
  gs->Member3 = inputs[2].input_vector[frame-1].input;
}

void render_game_state(int frame, GameState* gs){
//  printf("//====================================================//\n");
//  printf("Render Current Game State\n");
  printf("M1 = %d, M2 = %u, M3 = %u\n", gs->Member1, gs->Member2, gs->Member3);
}

int main(void) {
  PU_SESSION_CALLBACKS cb;
  PU_SESSION session;
  ENetHost* host;
  //int dt = 16667;
  int dt = 1;
  uint16_t test_input = 113;

  cb.restore_game_state = restore_game_state;
  cb.save_game_state = save_game_state;
  cb.render_game_state = render_game_state;
  cb.advance_game_state = advance_game_state;

  pu_initialize(&session);
  host = pu_create_host(&session);

  do {
    pu_update_network(&session, host);
    if (session.has_started){
      pu_determine_sync_frame(&session);
      if (pu_rollback_condition(&session)){
        //restore gamestate to sync frame
        cb.restore_game_state(session.sync_frame, &gs_vec, &gs);
        //use all the inputs since the last sync frame until the current frame to simulate
        for (int i = session.sync_frame + 1; i <= session.local_frame; i++) {
          ///update without rendering
          //update input to be used in the game
          pu_predict_remote_input(&session, i);
          //advance gamestate
          cb.advance_game_state(i, &gs, &session.player_input);
          //save gamestate
          cb.save_game_state(i, &gs_vec, &gs);
        }
      }
      if (pu_timesynced_condition(&session)){
        //increment local frame
        session.local_frame++;
        //get local player
        if (test_input > 254) {
          test_input = 0;
        }else{
          test_input++;
        }
        pu_predict_remote_input(&session, session.local_frame);
        pu_add_local_input(&session, host, test_input);
        //normal update with rendering
        //update input to be used in the game
        //advance gamestate
        cb.advance_game_state(session.local_frame, &gs, &session.player_input);
        //save gamestate
        cb.save_game_state(session.local_frame, &gs_vec, &gs);
        //render gamestate
        cb.render_game_state(session.local_frame, &gs, NULL);
      }
    }
    sleep(dt);
  } while(true);

  pu_destroy_host(host, &session);
  pu_deinitialize(&session);
  return 0;
}
