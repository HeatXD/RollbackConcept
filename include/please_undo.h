//Oh ye btw *X*X means not implemented or not done yet : Jamie
//-----------------------------------------------------------------------------
#ifndef PLEASE_UNDO_H
#define PLEASE_UNDO_H
#define ENET_IMPLEMENTATION
#include "enet.h"
#include <stdio.h>
//PLEASE_UNDO DEBUG
#define SHOW_DEBUG 1 // Show Debug messages
//Declaration Constants
#define MAX_ROLLBACK_FRAMES 10 // Specifies the maximum number of frames that can be resimulated
#define FRAME_ADVANTAGE_LIMIT 2 // Only allow the local client to get so far ahead of remote
#define INPUT_DELAY 1 // Should always be above 0
#define INITIAL_FRAME 0 //Specifies the initial frame the game starts in. Cannot rollback before this frame
#define DEFAULT_HOST_PORT 12344 // Default port for when the host creates a game
//Declaration Structs
typedef struct PU_SESSION{
  int local_frame;// Tracks the latest update frame.
  int remote_frame;// Tracks the latest frame received from the remote client
  int sync_frame;// Tracks the last frame where we synchronized the game state with the remote client. Never rollback before this frame
  int remote_frame_advantage; // Latest frame advantage received from the remote client
}PU_SESSION;
//Declaration Funcs
//Please Undo Session Functions
void pu_handle_rollbacks(PU_SESSION *session);// X*X*
void pu_determine_sync_frame(PU_SESSION *session);// X*X*
int pu_rollback_condition(PU_SESSION *session); // No need to rollback if we don't have a frame after the previous sync frame to synchronize to
int pu_timesynced_condition(PU_SESSION *session);// Function for syncing both players making the other wait
//Network Related Functions
void pu_update_network(ENetHost *player);// X*X*
int pu_check_error(int num);// Basic error checking function
void pu_print_debug(const char *message);// Print function that only sends messages when SHOW_DEBUG is enabled(1)
int pu_initialize(PU_SESSION *session); // Init Please Undo and Enet
void pu_deinitialize(); // De Init Please Undo
ENetHost* pu_create_host(int port);// Create Host
void pu_host_destroy(ENetHost *host);// Destroy Host
ENetHost* pu_client_create();// Create Client
void pu_client_destroy(ENetHost *client);// Destroy Client
//Save State Related Functions
void pu_save_state_callback(void (*func)(int), int frame_num);// callback to a save state function which takes a parameter to the current frame
void pu_restore_state_callback(void (*func)(int), int frame_num);// callback to a restore state function which takes a parameter to a previous frame
//-----------------------------------------------------------------------------
// Implementation
#ifdef PLEASE_UNDO_IMPL_H
void pu_determine_sync_frame(PU_SESSION *session){
  int final_frame = session->remote_frame; // We will only check inputs until the remote_frame, since we don't have inputs after.
  if (session->remote_frame > session->local_frame) {
    final_frame = session->remote_frame;  //Incase the remote client is ahead of local, don't check past the local frame.
  }
  //select frames from (sync_frame + 1) through final_frame and find the first frame where predicted and remote inputs don't match TODO
  //if (found_frame) {
  //  session->sync_frame = found_frame -1;  //The found frame is the first frame where inputs don't match, so assume the previous frame is synchronized
  //}else{
  //  session->sync_frame = final_frame; // All remote inputs matched the predicted inputs since the last synchronized frame
  //}
}
// No need to rollback if we don't have a frame after the previous sync frame to synchronize to
int pu_rollback_condition(PU_SESSION *session){
  if (session->local_frame > session->sync_frame && session->remote_frame > session->sync_frame) {
    return 1;
  } else {
    return 0;
  }
}
// Function for syncing both players making the other wait
int pu_timesynced_condition(PU_SESSION *session){
  int local_frame_advantage = session->local_frame - session->remote_frame;  // How far the client is ahead of the last reported frame by the remote client
  int frame_advantage_difference = local_frame_advantage - session->remote_frame_advantage;// How different is the frame advantage reported by the remote client and this one
  if (local_frame_advantage < MAX_ROLLBACK_FRAMES && frame_advantage_difference <= FRAME_ADVANTAGE_LIMIT) {// Only allow the local client to get so far ahead of remote
    return 1;
  }else{
    return 0;
  }
}
// Callback to a restore state function
void pu_restore_state_callback(void (*restore_func)(int), int frame_num){
  restore_func(frame_num);
}
// Callback to a save state function
void pu_save_state_callback(void (*save_func)(int), int frame_num){
  save_func(frame_num);
}
// Basic error checking function
int pu_check_error(int num){
  if (num != 0) {
      return 1;
  }else{
      return 0;
  }
}
//Print function that only sends messages when SHOW_DEBUG is enabled
void pu_print_debug(const char *message){
  if (SHOW_DEBUG) {
    printf(message);
  }
}
// Init Please undo and Enet
int pu_initialize(PU_SESSION *session){
  session->local_frame = INITIAL_FRAME;
  session->remote_frame = INITIAL_FRAME;
  session->sync_frame = INITIAL_FRAME;
  session->remote_frame_advantage = 0;
  pu_print_debug("Initialized PU session\n");
  if (!pu_check_error(enet_initialize())){
    pu_print_debug("Enet initialized\n");
    return 0;
  }else{
    pu_print_debug("Couldn't initialize Enet\n");
    return 1;
  }
}
// De-Init Enet
void pu_deinitialize(){
  enet_deinitialize();
  pu_print_debug("Closed Enet\n");
}
// Create Host
ENetHost* pu_host_create(int port){
  ENetAddress address = {0};
  address.host = ENET_HOST_ANY;
  address.port = DEFAULT_HOST_PORT;
  ENetHost *host = enet_host_create(&address,1,1,0,0);
  if (host == NULL) {
    pu_print_debug("An error occurred while creating a host\n");
    return host;
  }else{
    pu_print_debug("Host created successfully\n");
    return host;
  }
}
// Destroy Host
void pu_host_destroy(ENetHost *host){
  enet_host_destroy(host);
  pu_print_debug("Host destroyed\n");
}
// Create Client
ENetHost* pu_client_create(){
  ENetHost* client = enet_host_create(NULL,1,1,0,0);
  if (client == NULL) {
    pu_print_debug("An error occurred while creating a client\n");
    return client;
  }else{
    pu_print_debug("Client created successfully\n");
    return client;
  }
}
// Destroy Client
void pu_client_destroy(ENetHost *client){
  enet_host_destroy(client);
  pu_print_debug("Client destroyed\n");
}
//
#endif //PLEASE_UNDO_IMPL_H

#endif //PLEASE_UNDO_H
